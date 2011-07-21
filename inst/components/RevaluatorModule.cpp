#include "nsIGenericFactory.h"
#include "RevaluatorImpl.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(REvaluatorImpl)


#define MY_COMPONENT_CONTRACTID "@R/Revaluator;1"
#define MY_COMPONENT_CLASSNAME "An interface to the R evaluator"
/*#define MY_COMPONENT_CID  { 0x597a60b0, 0x5272, 0x4284, { 0x90, 0xf6, 0xe9, 0x6c, 0x24, 0x2d, 0x74, 0x6 } } */
#define MY_COMPONENT_CID  { 0x197a60b0, 0x1272, 0x1284, { 0x90, 0xf6, 0xe9, 0x6c, 0x24, 0x2d, 0x74, 0x6 } }

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
