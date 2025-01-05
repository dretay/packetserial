%module PacketDAO
%include "carrays.i"
%include "cdata.i"

%array_functions(char, charArray);
%cdata(char) 

%{
#define SWIG_FILE_WITH_INIT
/* Includes the header in the wrapper code */
#include "PacketDAO.h"
%}
/* Parse the header file to generate wrappers */
%include "PacketDAO.h"
