// "generic" is a reserved keyword in C++/CX code but is used in Freetype headers
// for a variable name. Redefine this keyword to avoid compilation errors in UWP.
#define generic HideGeneric