# Came from TU processing for an enum named nsXPTType::._20.
setClass('nsXPTType', contains = 'EnumValue')
`nsXPTTypeValues` = EnumDef('nsXPTType', structure(as.integer(c(0,
1,
2,
3,
4,
5,
6,
7,
8,
9,
10,
11,
12,
13,
14,
15,
16,
17,
18,
19,
20,
21,
22,
23,
24,
25)),
names = c("T_I8",
"T_I16",
"T_I32",
"T_I64",
"T_U8",
"T_U16",
"T_U32",
"T_U64",
"T_FLOAT",
"T_DOUBLE",
"T_BOOL",
"T_CHAR",
"T_WCHAR",
"T_VOID",
"T_IID",
"T_DOMSTRING",
"T_CHAR_STR",
"T_WCHAR_STR",
"T_INTERFACE",
"T_INTERFACE_IS",
"T_ARRAY",
"T_PSTRING_SIZE_IS",
"T_PWSTRING_SIZE_IS",
"T_UTF8STRING",
"T_CSTRING",
"T_ASTRING")))

setAs('numeric', 'nsXPTType', function(from)  asEnumValue(from, `nsXPTTypeValues`, 'nsXPTType', prefix = "T_"))
setAs('character', 'nsXPTType', function(from)  asEnumValue(from, `nsXPTTypeValues`, 'nsXPTType', prefix = "T_"))
setAs('integer', 'nsXPTType', function(from)  asEnumValue(from, `nsXPTTypeValues`, 'nsXPTType', prefix = "T_"))


`T_I8` <- GenericEnumValue('T_I8', 0, 'nsXPTType')
`T_I16` <- GenericEnumValue('T_I16', 1, 'nsXPTType')
`T_I32` <- GenericEnumValue('T_I32', 2, 'nsXPTType')
`T_I64` <- GenericEnumValue('T_I64', 3, 'nsXPTType')
`T_U8` <- GenericEnumValue('T_U8', 4, 'nsXPTType')
`T_U16` <- GenericEnumValue('T_U16', 5, 'nsXPTType')
`T_U32` <- GenericEnumValue('T_U32', 6, 'nsXPTType')
`T_U64` <- GenericEnumValue('T_U64', 7, 'nsXPTType')
`T_FLOAT` <- GenericEnumValue('T_FLOAT', 8, 'nsXPTType')
`T_DOUBLE` <- GenericEnumValue('T_DOUBLE', 9, 'nsXPTType')
`T_BOOL` <- GenericEnumValue('T_BOOL', 10, 'nsXPTType')
`T_CHAR` <- GenericEnumValue('T_CHAR', 11, 'nsXPTType')
`T_WCHAR` <- GenericEnumValue('T_WCHAR', 12, 'nsXPTType')
`T_VOID` <- GenericEnumValue('T_VOID', 13, 'nsXPTType')
`T_IID` <- GenericEnumValue('T_IID', 14, 'nsXPTType')
`T_DOMSTRING` <- GenericEnumValue('T_DOMSTRING', 15, 'nsXPTType')
`T_CHAR_STR` <- GenericEnumValue('T_CHAR_STR', 16, 'nsXPTType')
`T_WCHAR_STR` <- GenericEnumValue('T_WCHAR_STR', 17, 'nsXPTType')
`T_INTERFACE` <- GenericEnumValue('T_INTERFACE', 18, 'nsXPTType')
`T_INTERFACE_IS` <- GenericEnumValue('T_INTERFACE_IS', 19, 'nsXPTType')
`T_ARRAY` <- GenericEnumValue('T_ARRAY', 20, 'nsXPTType')
`T_PSTRING_SIZE_IS` <- GenericEnumValue('T_PSTRING_SIZE_IS', 21, 'nsXPTType')
`T_PWSTRING_SIZE_IS` <- GenericEnumValue('T_PWSTRING_SIZE_IS', 22, 'nsXPTType')
`T_UTF8STRING` <- GenericEnumValue('T_UTF8STRING', 23, 'nsXPTType')
`T_CSTRING` <- GenericEnumValue('T_CSTRING', 24, 'nsXPTType')
`T_ASTRING` <- GenericEnumValue('T_ASTRING', 25, 'nsXPTType')
