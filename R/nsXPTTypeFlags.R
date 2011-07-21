
setClass('nsXPTTypeFlags', contains = 'EnumValue')
`nsXPTTypeFlagsValues` = EnumDef('nsXPTTypeFlags', structure(as.integer(c(0,
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


setAs('numeric', 'nsXPTTypeFlags', function(from)  asEnumValue(from, `nsXPTTypeFlagsValues`, 'nsXPTTypeFlags', prefix = "T_"))
setAs('character', 'nsXPTTypeFlags', function(from)  asEnumValue(from, `nsXPTTypeFlagsValues`, 'nsXPTTypeFlags', prefix = "T_"))
setAs('integer', 'nsXPTTypeFlags', function(from)  asEnumValue(from, `nsXPTTypeFlagsValues`, 'nsXPTTypeFlags', prefix = "T_"))


`T_I8` <- GenericEnumValue('T_I8', 0, 'nsXPTTypeFlags')
`T_I16` <- GenericEnumValue('T_I16', 1, 'nsXPTTypeFlags')
`T_I32` <- GenericEnumValue('T_I32', 2, 'nsXPTTypeFlags')
`T_I64` <- GenericEnumValue('T_I64', 3, 'nsXPTTypeFlags')
`T_U8` <- GenericEnumValue('T_U8', 4, 'nsXPTTypeFlags')
`T_U16` <- GenericEnumValue('T_U16', 5, 'nsXPTTypeFlags')
`T_U32` <- GenericEnumValue('T_U32', 6, 'nsXPTTypeFlags')
`T_U64` <- GenericEnumValue('T_U64', 7, 'nsXPTTypeFlags')
`T_FLOAT` <- GenericEnumValue('T_FLOAT', 8, 'nsXPTTypeFlags')
`T_DOUBLE` <- GenericEnumValue('T_DOUBLE', 9, 'nsXPTTypeFlags')
`T_BOOL` <- GenericEnumValue('T_BOOL', 10, 'nsXPTTypeFlags')
`T_CHAR` <- GenericEnumValue('T_CHAR', 11, 'nsXPTTypeFlags')
`T_WCHAR` <- GenericEnumValue('T_WCHAR', 12, 'nsXPTTypeFlags')
`T_VOID` <- GenericEnumValue('T_VOID', 13, 'nsXPTTypeFlags')
`T_IID` <- GenericEnumValue('T_IID', 14, 'nsXPTTypeFlags')
`T_DOMSTRING` <- GenericEnumValue('T_DOMSTRING', 15, 'nsXPTTypeFlags')
`T_CHAR_STR` <- GenericEnumValue('T_CHAR_STR', 16, 'nsXPTTypeFlags')
`T_WCHAR_STR` <- GenericEnumValue('T_WCHAR_STR', 17, 'nsXPTTypeFlags')
`T_INTERFACE` <- GenericEnumValue('T_INTERFACE', 18, 'nsXPTTypeFlags')
`T_INTERFACE_IS` <- GenericEnumValue('T_INTERFACE_IS', 19, 'nsXPTTypeFlags')
`T_ARRAY` <- GenericEnumValue('T_ARRAY', 20, 'nsXPTTypeFlags')
`T_PSTRING_SIZE_IS` <- GenericEnumValue('T_PSTRING_SIZE_IS', 21, 'nsXPTTypeFlags')
`T_PWSTRING_SIZE_IS` <- GenericEnumValue('T_PWSTRING_SIZE_IS', 22, 'nsXPTTypeFlags')
`T_UTF8STRING` <- GenericEnumValue('T_UTF8STRING', 23, 'nsXPTTypeFlags')
`T_CSTRING` <- GenericEnumValue('T_CSTRING', 24, 'nsXPTTypeFlags')
`T_ASTRING` <- GenericEnumValue('T_ASTRING', 25, 'nsXPTTypeFlags')
