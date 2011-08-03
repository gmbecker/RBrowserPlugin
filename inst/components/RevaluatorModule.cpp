#include "nsIGenericFactory.h"
#include "RevaluatorImpl.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(REvaluatorImpl)


#define MY_COMPONENT_CONTRACTID "@R/Revaluator;1"
#define MY_COMPONENT_CLASSNAME "An interface to the R evaluator"
/*#define MY_COMPONENT_CID  { 0x597a60b0, 0x5272, 0x4284, { 0x90, 0xf6, 0xe9, 0x6c, 0x24, 0x2d, 0x74, 0x6 } } */
//#define MY_COMPONENT_CID  { 0x197a60b0, 0x1272, 0x1284, { 0x90, 0xf6, 0xe9, 0x6c, 0x24, 0x2d, 0x74, 0x6 } }
#define MY_COMPONENT_CID { 0x381cb19e, 0x6bd4, 0x489e, {0x81, 0xac, 0xe8, 0x8e, 0xe2, 0xe1, 0xf3, 0xc7, 0xe} }
static nsModuleComponentInfo components[] =
{
    {
       MY_COMPONENT_CLASSNAME, 
       MY_COMPONENT_CID,
       MY_COMPONENT_CONTRACTID,
       REvaluatorImplConstructor,
    }
};

NS_IMPL_NSGETMODULE("REvaluatorModule", components) 
