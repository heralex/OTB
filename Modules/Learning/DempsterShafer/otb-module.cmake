set(DOCUMENTATION "Data fusion needs a common theoretical framework to
manipulate and combine data sources having different natures. The mathematical theory of
dempster-Shafer, which is based on belief function and plausible reasoning, can
be used for this purpose. This module contains classes related to this theory (see
also majority voting module).")

otb_module(OTBDempsterShafer 
  DEPENDS
    OTBVectorDataBase
    OTBImageBase
    OTBVectorDataManipulation
    OTBSupervised
    OTBCommon
    OTBMathParser
    OTBFuzzy
  TEST_DEPENDS
    OTBTestKernel
    OTBVectorDataIO
  
  DESCRIPTION
    "${DOCUMENTATION}"
  )
