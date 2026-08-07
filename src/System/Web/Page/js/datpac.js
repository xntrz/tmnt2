/////////////////////////////////////////////////////////////////////////////////
//
//  Handling load & unload emscripten .data pacs at runtime. (test)
//
//  Callbacks prototypes & description for C code:
//
//      void onDatpacLoad(const char* filename, int result)
//          Fires at data pac load result. To determinate fallure or success
//          check 'result' - equal 1 on success or 0 on failure.
//
/////////////////////////////////////////////////////////////////////////////////  

const loadedDatPacs = {};

function datpacLog(str) {
    console.log("[datpac] " + str);
}

function datpacNoteLoad(filename, isSuccess) {
    Module.ccall(
        "onDatpacLoad",
        "void", ["string", "number"],
        [filename, isSuccess]
    );
}

function datpacLoad(filename) {
    if (loadedDatPacs[filename]) {
        datpacLog(filename + " already loaded");
        return;
    }

    fetch(filename + ".js.metadata")
        .then(response => {
            if (!response.ok) {
                throw new Error("cannot load metadata");
            }
            return response.json();
        })
        .then(metadata => {
            const script = document.createElement("script");
            script.src = (filename + ".js");

            script.onload = () => {
                const filePaths = metadata.files.map(file => file.filename);
                loadedDatPacs[filename] = {
                    files: filePaths,
                    script: script
                };

                datpacNoteLoad(filename, 1);
            };

            script.onerror = () => {
                datpacNoteLoad(filename, 0);
            };

            document.head.appendChild(script);
        })
        .catch(error => {
            datpacLog("error while loading " + filename + " : " + error);
            datpacNoteLoad(filename, 0);
        });
}

function datpacUnload(filename) {
    const datpacInfo = loadedDatPacs[filename];

    if (!datpacInfo) {
        datpacLog(filename + " is not loaded");
        return false;
    }

    try {
        datpacInfo.files.forEach(filePath => {
            if (FS.analyzePath(filePath).exists) {
                FS.unlink(filePath);
            }
        });

        if (datpacInfo.script) {
            datpacInfo.script.remove();
        }

        delete loadedDatPacs[filename];
        return true;
    } catch (error) {
        console.error("error while unloading " + filename + " : " + error);
        return false;
    }
}

function datpacIsLoaded(filename) {
    const datpacInfo = loadedDatPacs[filename];

    if (!datpacInfo) {
        return 0;
    }

    const files = datpacInfo.files;

    if (!files || files.length === 0) {
        return 0;
    }

    try {
        const firstFile = files[0];
        const lastFile = files[files.length - 1];

        const firstExists = FS.analyzePath(firstFile).exists;
        const lastExists = FS.analyzePath(lastFile).exists;

        return (firstExists && lastExists) ? 1 : 0;
    } catch (e) {
        return 0;
    }
}