#ifdef BUILD_RFIREFOX
SEXP JSRefToR(JSContext *jscon, jsval *jsobj);
SEXP JSArrayToList(JSContext *jscon, JSObject *array, int simplify);
#endif //BUILD_RFIREFOX
