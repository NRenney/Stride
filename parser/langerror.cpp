#include "langerror.h"

#include <sstream>

LangError::LangError()
{
  
}

std::string LangError::getErrorText() {
    std::string errorText;
    switch(type) {
    case Syntax:
        errorText = "Syntax Error: Unexpected character '" + errorTokens[0] + "'";
        break;
    case UnknownType:
        errorText = "Unknown Type Error. Type '" + errorTokens[0] + "' not recognized.";
        break;
    case InvalidType:
        errorText = "Invalid Type Error";
        break;
    case InvalidPort:
        errorText = "Invalid port Error";
        break;
    case InvalidPortType:
        errorText = "Invalid port type Error. Port '" + errorTokens[0]
                + "' in Block '" + errorTokens[1]
                + "' expects '" + errorTokens[2] + "'";
        break;
    case IndexMustBeInteger:
        errorText = "Index to array must be integer ";
        break;
    case BundleSizeMismatch:
        errorText = "Bundle Size Mismatch Error";
        break;
    case ArrayIndexOutOfRange:
        errorText = "Array Index out of Range Error";
        break;
    case DuplicateSymbol:
        errorText = "Duplicate Symbol Error";
        break;
    case InconsistentList:
        errorText = "Inconsistent List Error";
        break;
    case UndeclaredSymbol:
        errorText = "Undeclared Symbol '" + errorTokens[0] + "'";
        break;
    case None:
    default:
        break;
    }

    std::stringstream lineNumString;
    lineNumString << lineNumber;

    errorText += " in line " + lineNumString.str();
    return errorText;
}
