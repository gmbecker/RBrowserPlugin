setClass('nsXPTCVariantFlags', contains = 'EnumValue')
`nsXPTCVariantFlagsValues` = EnumDef('nsXPTCVariantFlags', structure(as.integer(c(1,
2,
4,
8,
16,
32,
64)),
names = c("PTR_IS_DATA",
"VAL_IS_ALLOCD",
"VAL_IS_IFACE",
"VAL_IS_ARRAY",
"VAL_IS_DOMSTR",
"VAL_IS_UTF8STR",
"VAL_IS_CSTR")))


setAs('numeric', 'nsXPTCVariantFlags', function(from)  asEnumValue(from, `nsXPTCVariantFlagsValues`, 'nsXPTCVariantFlags'))
setAs('character', 'nsXPTCVariantFlags', function(from)  asEnumValue(from, `nsXPTCVariantFlagsValues`, 'nsXPTCVariantFlags'))
setAs('integer', 'nsXPTCVariantFlags', function(from)  asEnumValue(from, `nsXPTCVariantFlagsValues`, 'nsXPTCVariantFlags'))


`PTR_IS_DATA` <- GenericEnumValue('PTR_IS_DATA', 1, 'nsXPTCVariantFlags')
`VAL_IS_ALLOCD` <- GenericEnumValue('VAL_IS_ALLOCD', 2, 'nsXPTCVariantFlags')
`VAL_IS_IFACE` <- GenericEnumValue('VAL_IS_IFACE', 4, 'nsXPTCVariantFlags')
`VAL_IS_ARRAY` <- GenericEnumValue('VAL_IS_ARRAY', 8, 'nsXPTCVariantFlags')
`VAL_IS_DOMSTR` <- GenericEnumValue('VAL_IS_DOMSTR', 16, 'nsXPTCVariantFlags')
`VAL_IS_UTF8STR` <- GenericEnumValue('VAL_IS_UTF8STR', 32, 'nsXPTCVariantFlags')
`VAL_IS_CSTR` <- GenericEnumValue('VAL_IS_CSTR', 64, 'nsXPTCVariantFlags')

#####################
