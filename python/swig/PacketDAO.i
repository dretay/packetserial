%module PacketDAO
%include "carrays.i"
%include "cdata.i"
%array_functions(char, charArray);
%cdata(char) 
%{
#define SWIG_FILE_WITH_INIT
#include "PacketDAO.h"
%}
%include "PacketDAO.h"
