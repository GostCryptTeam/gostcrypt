import QtQuick 2.7

QtObject {
    property int type: 0 //default

    property color shadow:          style(["#000000", "#242424"])
    property color dark:            style(["#303030", "#E0E0E0"])
    property color darkSecond:      style(["#2a2a2a", "#EDEDED"])
    property color darkThird:       style(["#272727", "#B0B0B0"])
    property color textLowOpacity:  style(["#454545", "#242424"])
    property color text:            style(["#e1e1e1", "#242424"])
    property color textLight:       style(["#e1e1e1", "#424242"])
    property color textVeryLight:   style(["#e1e1e1", "#424242"])
    property color green:           "#719c24"
    property color greenHover:      "#7ba430"
    property color greenDark:       "#597d1c"
    property color blue:            "#2f88a7"
    property color blueHover:       "#3d97b6"
    property color bluePress:       "#266f88"
    property color border:          style(["#202020", "#C7C7C7"])
    property color darkInput:       style(["#181818", "#C7C7C7"])
    property color hoverItemMenu:   style(["#404040", "#C7C7C7"])
    property color bkCheckBox:      "#191919"
    property color grayWizard:      "#2e2e2e"
    property color grayWizardDark:  "#2b2b2b"
    property color round:           "#545454"
    property color roundFilled:     "#dfdfdf"
    property color borderInput:     "#333333"

    function style(colors)
    {
        return colors[type];
    }
}

