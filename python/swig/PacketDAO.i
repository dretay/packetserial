%module PacketDAO
%include "carrays.i"
%include "cdata.i"

%array_functions(int, intArray);

%{
#define SWIG_FILE_WITH_INIT
/* Includes the header in the wrapper code */
#include "PacketDAO.h"
%}
/* Parse the header file to generate wrappers */
%include "PacketDAO.h"
