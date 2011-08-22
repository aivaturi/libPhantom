// This allows creating a new web page using the construct "new WebPage",
// which feels more natural than "phantom.createWebPage()".
window.WebPage = function() {
    var page = phantom.createWebPage(),
        handlers = {};

    function defineSetter(handlerName, signalName) {
        page.__defineSetter__(handlerName, function(f) {
            if (handlers && typeof handlers[signalName] === 'function') {
                try {
                    this[signalName].disconnect(handlers[signalName]);
                } catch (e) {}
            }
            handlers[signalName] = f;
            this[signalName].connect(handlers[signalName]);
        });
    };

    // deep copy
    page.settings = JSON.parse(JSON.stringify(phantom.defaultPageSettings));

    defineSetter("onLoadStarted", "loadStarted");

    defineSetter("onLoadFinished", "loadFinished");

    defineSetter("onResourceRequested", "resourceRequested");

    defineSetter("onResourceReceived", "resourceReceived");

    defineSetter("onAlert", "javaScriptAlertSent");

    defineSetter("onConsoleMessage", "javaScriptConsoleMessageSent");

    page.open = function () {
        if (arguments.length === 1) {
            this.openUrl(arguments[0], 'get', this.settings);
            return;
        }
        if (arguments.length === 2 && typeof arguments[1] === 'function') {
            this.onLoadFinished = arguments[1];
            this.openUrl(arguments[0], 'get', this.settings);
            return;
        } else if (arguments.length === 2) {
            this.openUrl(arguments[0], arguments[1], this.settings);
            return;
        } else if (arguments.length === 3 && typeof arguments[2] === 'function') {
            this.onLoadFinished = arguments[2];
            this.openUrl(arguments[0], arguments[1], this.settings);
            return;
        } else if (arguments.length === 3) {
            this.openUrl(arguments[0], {
                operation: arguments[1],
                data: arguments[2]
                }, this.settings);
            return;
        } else if (arguments.length === 4) {
            this.onLoadFinished = arguments[3];
            this.openUrl(arguments[0], {
                operation: arguments[1],
                data: arguments[2]
                }, this.settings);
            return;
        }
        throw "Wrong use of WebPage#open";
    };

    page.includeJs = function(scriptUrl, onScriptLoaded) {
        // Register temporary signal handler for 'alert()'
        this.javaScriptAlertSent.connect(function(msgFromAlert) {
            if ( msgFromAlert === scriptUrl ) {
                // Resource loaded, time to fire the callback
                onScriptLoaded(scriptUrl);
                // And disconnect the signal handler
                try {
                    this.javaScriptAlertSent.disconnect(arguments.callee);
                } catch (e) {}
            }
        });

        // Append the script tag to the body
        this._appendScriptElement(scriptUrl);
    };

    page.destroy = function() {
        phantom._destroy(page);
    };

    return page;
};

// window.fs
// JavaScript "shim" to throw exceptions in case a critical operation fails.

/** Open and return a "file" object.
 * It will throw exception if it fails.
 *
 * @param path Path of the file to open
 * @param mode Open Mode. A string made of 'r', 'w', 'a/+' characters.
 * @return "file" object
 */
window.fs.open = function(path, mode) {
    var file = window.fs._open(path, mode);
    if (file) {
        return file;
    }
    throw "Unable to open file '"+ path +"'";
};

/** Open, read and return content of a file.
 * It will throw an exception if it fails.
 *
 * @param path Path of the file to read from
 * @return file content 
 */
window.fs.read = function(path) {
    var f = fs.open(path, 'r'),
        content = f.read();

    f.close();
    return content;
};

/** Open and write content to a file
 * It will throw an exception if it fails.
 *
 * @param path Path of the file to read from
 * @param content Content to write to the file
 * @param mode Open Mode. A string made of 'w' or 'a / +' characters.
 */
window.fs.write = function(path, content, mode) {
    var f = fs.open(path, mode);

    f.write(content);
    f.close();
};

/** Return the size of a file, in bytes.
 * It will throw an exception if it fails.
 *
 * @param path Path fo the file to read the size of
 * @return File size in bytes
 */
window.fs.size = function(path) {
    var size = fs._size(path);
    if (size !== -1) {
        return size;
    }
    throw "Unable to read file '"+ path +"' size";
};
