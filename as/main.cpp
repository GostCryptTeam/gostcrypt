#include <QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QBuffer>
#define dumpval(x) qDebug()<<#x<<'='<<x


#define SERIALIZABLE(ClassName) \
	Q_DECLARE_METATYPE(ClassName) \
	QDataStream & operator << (QDataStream & out, const ClassName & Valeur); \
	QDataStream & operator >> (QDataStream & in, ClassName & Valeur); \
	QDataStream & operator >> (QDataStream & in, QSharedPointer<ClassName> & Valeur) { \
		Valeur.reset(new ClassName); \
		in >> *Valeur; \
		return in; \
	} \
	QDataStream & operator << (QDataStream & out, const QSharedPointer<ClassName> & Valeur) { \
		out << *Valeur; \
		return out; \
	} \
	Q_DECLARE_METATYPE(QSharedPointer<ClassName>)


struct BaseClass {
	qint32 a;
};
SERIALIZABLE(BaseClass)

struct ChildCLass1 : BaseClass {
	qint32 b;
};
SERIALIZABLE(ChildCLass1)

struct ChildCLass2 : BaseClass {
	qreal c;
};
SERIALIZABLE(ChildCLass2)

QDataStream & operator << (QDataStream & out, const BaseClass & Valeur) {
	out << Valeur.a;
	return out;
}
QDataStream & operator >> (QDataStream & in, BaseClass & Valeur) {
	in >> Valeur.a;
	return in;
}

QDataStream & operator << (QDataStream & out, const ChildCLass1 & Valeur) {
	out << static_cast<const BaseClass&>(Valeur);
	out << Valeur.b;
	return out;
}
QDataStream & operator >> (QDataStream & in, ChildCLass1 & Valeur) {
	in >> static_cast<BaseClass&>(Valeur);
	in >> Valeur.b;
	return in;
}

QDataStream & operator << (QDataStream & out, const ChildCLass2 & Valeur) {
	out << static_cast<const BaseClass&>(Valeur);
	out << Valeur.c;
	return out;
}
QDataStream & operator >> (QDataStream & in, ChildCLass2 & Valeur) {
	in >> static_cast<BaseClass&>(Valeur);
	in >> Valeur.c;
	return in;
}

int parentProcess(int argc, char *argv[]);
int childProcess(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	qSetMessagePattern("%{appname}: %{message}");
	if(argc <= 1) {
		parentProcess(argc, argv);
	} else {
		childProcess(argc, argv);
	}
}

void send(QCoreApplication *a, QDataStream &stream, QVariant v) {
	stream << v;
	a->processEvents();
	qDebug() << "sent";
}

void printRead(QCoreApplication *a, QDataStream &stream) {
	qDebug() << "reading";
	QVariant v;
	while(v.isNull()) {
		stream >> v;
		QThread::msleep(100);
		a->processEvents();
	}
	if(v.canConvert<ChildCLass1>()) {
		ChildCLass1 c1Out;
		c1Out = v.value<ChildCLass1>();
		qDebug() << c1Out.a << c1Out.b;
	} else if(v.canConvert<ChildCLass2>()) {
		ChildCLass2 c2Out;
		c2Out = v.value<ChildCLass2>();
		qDebug() << c2Out.a << c2Out.c;
	} else if(v.canConvert<QSharedPointer<ChildCLass1>>()) {
		QSharedPointer<ChildCLass1> p;
		p = v.value<QSharedPointer<ChildCLass1>>();
		qDebug() << p->a << p->b;
	}
	qDebug()<< "done";
}

int parentProcess(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	/* init */
	qRegisterMetaTypeStreamOperators<BaseClass>("BaseClass");
	qRegisterMetaTypeStreamOperators<ChildCLass1>("ChildCLass1");
	qRegisterMetaTypeStreamOperators<ChildCLass2>("ChildCLass2");
	qRegisterMetaTypeStreamOperators<QSharedPointer<ChildCLass1>>("QSharedPointer<ChildCLass1>");
	qDebug() << "launched";

	QCoreApplication::setApplicationName("parent");
	qDebug()<<"started";
	QProcess p;
	p.setProgram(QCoreApplication::applicationFilePath());
	p.setArguments({"slave"});
	p.setProcessChannelMode(QProcess::ForwardedErrorChannel);
	p.start();
	p.waitForStarted();
	qDebug() << p.pid();
	QDataStream stream(&p);


	//*
	ChildCLass1 c1;
	c1.a = 2;
	c1.b = 3;
	ChildCLass2 c2;
	c2.a = 8;
	c2.c = 3;
	send(&a, stream, QVariant::fromValue(c1));
	send(&a, stream, QVariant::fromValue(c2));
	printRead(&a, stream);
	printRead(&a, stream);
	send(&a, stream, QVariant::fromValue(c1));
	printRead(&a, stream);

	p.close();
	p.waitForFinished();

	return 0;
}

int childProcess(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName("child");
	/* init */
	qRegisterMetaTypeStreamOperators<BaseClass>("BaseClass");
	qRegisterMetaTypeStreamOperators<ChildCLass1>("ChildCLass1");
	qRegisterMetaTypeStreamOperators<ChildCLass2>("ChildCLass2");
	qRegisterMetaTypeStreamOperators<QSharedPointer<ChildCLass1>>("QSharedPointer<ChildCLass1>");

	qDebug() << "launched";
	QFile inputFile, outputFile;

	QDataStream inputStream(&inputFile), outputStream(&outputFile);
	QVariant v;
	inputFile.open(stdin, QFile::ReadOnly);
	outputFile.open(stdout, QFile::WriteOnly);

	do {
		qDebug() << "reading...";
		inputStream >> v;
		qDebug() << "read" << v;

		if(v.canConvert<ChildCLass1>()) {
			ChildCLass1 c1Out;
			c1Out = v.value<ChildCLass1>();
			qDebug() << c1Out.a << c1Out.b;
			c1Out.a *= 2;
			c1Out.b *= 2;
			outputStream << QVariant::fromValue(c1Out);
			outputFile.flush();
		} else if(v.canConvert<ChildCLass2>()) {
			ChildCLass2 c2Out;
			c2Out = v.value<ChildCLass2>();
			qDebug() << c2Out.a << c2Out.c;
			c2Out.a *= 2;
			c2Out.c *= 2;
			outputStream << QVariant::fromValue(c2Out);
			outputFile.flush();
		} else if(v.canConvert<QSharedPointer<ChildCLass1>>()) {
			QSharedPointer<ChildCLass1> p;
			p = v.value<QSharedPointer<ChildCLass1>>();
			qDebug() << p->a << p->b;
			p->a *= 2;
			p->b *= 2;
			outputStream << QVariant::fromValue(p);
			outputFile.flush();
		}
		a.processEvents();
	} while (inputStream.status() == QDataStream::Ok);

	return 0;
}
