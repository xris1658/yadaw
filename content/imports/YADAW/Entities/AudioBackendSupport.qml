import QtQml

QtObject {
    enum Backend {
        Off,
        AudioGraph,
        ALSA
    }
    function isBackendSupported(backend): bool {
        return true;
    }
}