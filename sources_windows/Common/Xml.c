/**
 *
 *	\file Xml.c
 *	\brief Manage xml functions
 *	\version 1.3.1
 *	\date 14/12/2016
 *
 */

/**
 *	
 *	Copyright (c) 2005-2010 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include <windows.h>
#include <stdio.h>
#include "Xml.h"


/**
 *
 *	\fn static BOOL BeginsWith (char *string, char *subString)
 *	\brief Return true if the string start with the substring, false else.
 *	\param[in] char * subString
 *	\param[in] char * string
 *	\return BOOL TRUE if (*subString == 0)
 *			FALSE if (*string == 0)
 *			FALSE else
 *
 */
static BOOL BeginsWith (char *string, char *subString)
{
	while (*string++ == *subString++)
	{
		if (*subString == 0) return TRUE;
		if (*string == 0) return FALSE;
	}

	return FALSE;
}


/**
 *
 *	\fn char *XmlNextNode (char *xmlNode)
 *	\brief Get the xml next node
 *	\param[in] char * xmlNode
 *	\return char * t if (t[1] != '/')
 *			NULL else
 *
 */
char *XmlNextNode (char *xmlNode)
{
	char *t = xmlNode + 1;
	while ((t = strchr (t, '<')) != NULL)
	{
		if (t[1] != '/')
			return t;

		t++;
	}

	return NULL;
}


/**
 *
 *	\fn char *XmlFindElement (char *xmlNode, char *nodeName)
 *	\brief Find element in xml
 *	\param[in] char * nodeName
 *	\param[in] char * xmlNode
 *	\return char * t if (BeginsWith (t + 1, nodeName) && (t[nameLen + 1] == '>' || t[nameLen + 1] == ' ')
 *			NULL else
 *
 */
char *XmlFindElement (char *xmlNode, char *nodeName)
{
	char *t = xmlNode;
	size_t nameLen = strlen (nodeName);

	do
	{
		if (BeginsWith (t + 1, nodeName)
			&& (t[nameLen + 1] == '>'
			|| t[nameLen + 1] == ' ')) return t;

	} while (t = XmlNextNode (t));

	return NULL;
}


/**
 *
 *	\fn char *XmlFindElementByAttributeValue (char *xml, char *nodeName, char *attrName, char *attrValue)
 *	\brief Find element in xml by attribute value
 *	\param[in] char * attrValue
 *	\param[in] char * attrName
 *	\param[in] char * nodeName
 *	\param[in] char * xml
 *	\return char * xml if (strcmp (attr, attrValue) == 0)
 *			NULL else
 *
 */
char *XmlFindElementByAttributeValue (char *xml, char *nodeName, char *attrName, char *attrValue)
{
	char attr[2048];

	while (xml = XmlFindElement (xml, nodeName))
	{
		XmlGetAttributeText (xml, attrName, attr, sizeof (attr));
		if (strcmp (attr, attrValue) == 0)
			return xml;

		xml++;
	}

	return NULL;
}


/**
 *
 *	\fn char *XmlGetAttributeText (char *xmlNode, char *xmlAttrName, char *xmlAttrValue, int xmlAttrValueSize)
 *	\brief Get next xml next attrbute text
 *	\param[in] int xmlAttrValueSize
 *	\param[in] char * xmlAttrValue
 *	\param[in] char * xmlAttrName
 *	\param[in] char * xmlNode
 *	\return char * NULL if (t[0] != '<')
 *			NULL if (e == NULL)
 *			xmlAttrValue else
 *
 */
char *XmlGetAttributeText (char *xmlNode, char *xmlAttrName, char *xmlAttrValue, int xmlAttrValueSize)
{
	char *t = xmlNode;
	char *e = xmlNode;
	int l = 0;

	xmlAttrValue[0] = 0;
	if (t[0] != '<') return NULL;

	e = strchr (e, '>');
	if (e == NULL) return NULL;

	while ((t = strstr (t, xmlAttrName)) && t < e)
	{
		char *o = t + strlen (xmlAttrName);
		if (t[-1] == ' '
			&&
			(BeginsWith (o, "=\"")
			|| BeginsWith (o, "= \"")
			|| BeginsWith (o, " =\"")
			|| BeginsWith (o, " = \""))
			)
			break;

		t++;
	}

	if (t == NULL || t > e) return NULL;

	t = strchr (t, '"') + 1;
	e = strchr (t, '"');
	l = (int)(e - t);
	if (e == NULL || l > xmlAttrValueSize) return NULL;

	memcpy (xmlAttrValue, t, l);
	xmlAttrValue[l] = 0;

	return xmlAttrValue;
}


/**
 *
 *	\fn char *XmlGetNodeText (char *xmlNode, char *xmlText, int xmlTextSize)
 *	\brief Get next xml node text
 *	\param[in] int xmlTextSize
 *	\param[in] char * xmlText
 *	\param[in] char * xmlNode
 *	\return char * NULL if (t[0] != '<')
 *			NULL if (t == (char *)1)
 *			NULL if (e == NULL)
 *			NULL if (e == NULL || l > xmlTextSize)
 *			t else
 *
 */
char *XmlGetNodeText (char *xmlNode, char *xmlText, int xmlTextSize)
{
	char *t = xmlNode;
	char *e = xmlNode + 1;
	int l = 0, i = 0, j = 0;

	xmlText[0] = 0;

	if (t[0] != '<')
		return NULL;

	t = strchr (t, '>') + 1;
	if (t == (char *)1) return NULL;

	e = strchr (e, '<');
	if (e == NULL) return NULL;

	l = (int)(e - t);
	if (e == NULL || l > xmlTextSize) return NULL;

	while (i < l)
	{
		if (BeginsWith (&t[i], "&lt;"))
		{
			xmlText[j++] = '<';
			i += 4;
			continue;
		}
		if (BeginsWith (&t[i], "&gt;"))
		{
			xmlText[j++] = '>';
			i += 4;
			continue;
		}
		if (BeginsWith (&t[i], "&amp;"))
		{
			xmlText[j++] = '&';
			i += 5;
			continue;
		}
		xmlText[j++] = t[i++];
	}
	xmlText[j] = 0;

	return t;
}


/**
 *
 *	\fn char *XmlQuoteText (const char *textSrc, char *textDst, int textDstMaxSize)
 *	\brief Quote xml text
 *	\param[in] int textDstMaxSize
 *	\param[in] char * textDst
 *	\param[in] const char * textSrc
 *	\return char * NULL if (textDstMaxSize == 0)
 *			NULL switch (c) case '&' if (textDst + 6 > textDstLast)
 *			NULL switch (c) case '>' if (textDst + 5 > textDstLast)
 *			NULL switch (c) case '<' if (textDst + 5 > textDstLast)
 *			NULL if (textDst > textDstLast)
 *			textDst else
 *
 */
char *XmlQuoteText (const char *textSrc, char *textDst, int textDstMaxSize)
{
	char *textDstLast = textDst + textDstMaxSize - 1;

	if (textDstMaxSize == 0)
		return NULL;

	while (*textSrc != 0 && textDst <= textDstLast) 
	{
		char c = *textSrc++;
		switch (c)
		{
		case '&':
			if (textDst + 6 > textDstLast)
				return NULL;
			strcpy (textDst, "&amp;");
			textDst += 5;
			continue;

		case '>':
			if (textDst + 5 > textDstLast)
				return NULL;
			strcpy (textDst, "&gt;");
			textDst += 4;
			continue;

		case '<':
			if (textDst + 5 > textDstLast)
				return NULL;
			strcpy (textDst, "&lt;");
			textDst += 4;
			continue;

		default:
			*textDst++ = c;
		}
	}

	if (textDst > textDstLast)
		return NULL;

	*textDst = 0;
	return textDst;
}


/**
 *
 *	\fn int XmlWriteHeader (FILE *file)
 *	\brief Write xml header
 *	\param[in] FILE * file
 *	\return int fputs ("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<GostCrypt>", file)
 *
 */
int XmlWriteHeader (FILE *file)
{
	return fputs ("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<GostCrypt>", file);
}


/**
 *
 *	\fn int XmlWriteHeaderW (FILE *file)
 *	\brief Write xml header
 *	\param[in] FILE * file
 *	\return int fputws (L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<GostCrypt>", file)
 *
 */
int XmlWriteHeaderW (FILE *file)
{
	return fputws (L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<GostCrypt>", file);
}


/**
 *
 *	\fn int XmlWriteFooter (FILE *file)
 *	\brief Write xml footer
 *	\param[in] FILE * file
 *	\return int fputs ("\n</GostCrypt>", file)
 *
 */
int XmlWriteFooter (FILE *file)
{
	return fputs ("\n</GostCrypt>", file);
}


/**
 *
 *	\fn int XmlWriteFooterW (FILE *file)
 *	\brief Write xml footer
 *	\param[in] FILE * file
 *	\return int fputws (L"\n</GostCrypt>", file)
 *
 */
int XmlWriteFooterW (FILE *file)
{
	return fputws (L"\n</GostCrypt>", file);
}
