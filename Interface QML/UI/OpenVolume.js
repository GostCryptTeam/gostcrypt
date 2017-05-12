function finishCreation(component, object, parent) {
    if (component.status == Component.Ready) {
        object = component.createObject(parent, {childOf: parent});
        if (object == null) {
            // Error Handling
            console.log("Error creating object");
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
    return object;
}
