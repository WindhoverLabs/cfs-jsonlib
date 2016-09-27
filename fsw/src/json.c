/*

Copyright (c) 2016, Windhover Labs
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "../../../jsonlib/fsw/src/json.h"

#include "json_api.h"
#include <string.h>
#include "cfe.h"

#include "../../../jsonlib/fsw/src/json_version.h"


int32 JSON_LibInit(void)
{
    OS_printf ("JSON Lib Initialized.  Version %d.%d.%d.%d",
                JSON_MAJOR_VERSION,
				JSON_MINOR_VERSION,
				JSON_REVISION,
				JSON_MISSION_REV);

    return OS_SUCCESS;

}


int32 JSON_Alloc(JSON_Object** JsonObject, CFE_ES_MemHandle_t *HandlePtr)
{
	int32 iStatus = CFE_SUCCESS;

	/* Allocate a new queue entry from the CF memory pool.*/
    iStatus = CFE_ES_GetPoolBuf((uint32 **)JsonObject, HandlePtr,  sizeof(JSON_Object));
    if(iStatus > 0)
    {
    	JSON_Object *newObj = *JsonObject;
    	newObj->Start = 0;
    	newObj->End = 0;
    	newObj->HandlePtr =  HandlePtr;
    	newObj->IsArray = FALSE;
    	newObj->Parent = 0;
    }

    return iStatus;
}



int32  JSON_Dealloc(JSON_Object* JsonObject)
{
	JSON_Param *jsonParam = JsonObject->Start;

	while(jsonParam != 0)
	{
		switch(jsonParam->ParamType)
		{
			case JSON_PARAM_TYPE_STRING:
			case JSON_PARAM_TYPE_NUMBER:
				CFE_ES_PutPoolBuf(JsonObject->HandlePtr, (uint32 *)jsonParam->ParamValue.StringParam );
				break;

			case JSON_PARAM_TYPE_OBJECT:
			{
				JSON_Dealloc(jsonParam->ParamValue.ObjectParam);
				break;
			}

			case JSON_PARAM_TYPE_ARRAY:
			{
				/* TODO */
				break;
			}

			default:
				/* TODO */
				break;
		}

		CFE_ES_PutPoolBuf(JsonObject->HandlePtr, (uint32 *)jsonParam->ParamName );
		JSON_Param *nextParam = jsonParam->Next;
		CFE_ES_PutPoolBuf(JsonObject->HandlePtr, (uint32 *)jsonParam );
		jsonParam = nextParam;
	}

	CFE_ES_PutPoolBuf(JsonObject->HandlePtr, (uint32 *)JsonObject);

	return CFE_SUCCESS;
}



int32  JSON_AddString(JSON_Object *Parent, const char *ParamName, const char *ParamValue)
{
	int32 iStatus = CFE_SUCCESS;

	JSON_Param *newParam;

	if(Parent->Start == 0)
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->Start, Parent->HandlePtr,  sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }

	    Parent->End = Parent->Start;
	}
	else
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->End->Next, Parent->HandlePtr, sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }
	    Parent->End = Parent->End->Next;
	}

	newParam = Parent->End;

	newParam->ParamType = JSON_PARAM_TYPE_STRING;
	uint32 valLength = strlen(ParamValue) + 1;
    iStatus = CFE_ES_GetPoolBuf((uint32 **)&(newParam->ParamValue.StringParam), Parent->HandlePtr, valLength);
    if(iStatus <= 0)
    {
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)Parent->End);
    	return 0;
    }
    strcpy(newParam->ParamValue.StringParam, ParamValue);
	newParam->Next = 0;

	uint32 nameLength = strlen(ParamName) + 1;
    iStatus = CFE_ES_GetPoolBuf((uint32 **)&(newParam->ParamName), Parent->HandlePtr, nameLength);
    if(iStatus <= 0)
    {
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)newParam->ParamValue.StringParam);
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)Parent->End);
    	return 0;
    }
    strcpy(newParam->ParamName, ParamName);

    return CFE_SUCCESS;
}



int32  JSON_AddNumber(JSON_Object *Parent, const char *ParamName, const char *ParamValue)
{
	int32 iStatus = CFE_SUCCESS;

	JSON_Param *newParam;

	if(Parent->Start == 0)
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->Start, Parent->HandlePtr,  sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }

	    Parent->End = Parent->Start;
	}
	else
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->End->Next, Parent->HandlePtr, sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }
	    Parent->End = Parent->End->Next;
	}

	newParam = Parent->End;

	newParam->ParamType = JSON_PARAM_TYPE_NUMBER;
	uint32 valLength = strlen(ParamValue) + 1;
    iStatus = CFE_ES_GetPoolBuf((uint32 **)&(newParam->ParamValue.StringParam), Parent->HandlePtr, valLength);
    if(iStatus <= 0)
    {
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)Parent->End);
    	return 0;
    }
    strcpy(newParam->ParamValue.StringParam, ParamValue);
	newParam->Next = 0;

	uint32 nameLength = strlen(ParamName) + 1;
    iStatus = CFE_ES_GetPoolBuf((uint32 **)&(newParam->ParamName), Parent->HandlePtr, nameLength);
    if(iStatus <= 0)
    {
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)newParam->ParamValue.StringParam);
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)Parent->End);
    	return 0;
    }
    strcpy(newParam->ParamName, ParamName);

    return CFE_SUCCESS;
}



int32 JSON_AddFloat(JSON_Object *Parent, const char *ParamName, float ParamValue)
{
	return -1;
}



int32 JSON_AddUint32(JSON_Object *Parent, const char *ParamName, uint32 ParamValue)
{
	int32 iStatus = CFE_SUCCESS;

	JSON_Param *newParam;

	if(Parent->Start == 0)
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->Start, Parent->HandlePtr,  sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }

	    Parent->End = Parent->Start;
	}
	else
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->End->Next, Parent->HandlePtr, sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }
	    Parent->End = Parent->End->Next;
	}

	newParam = Parent->End;

	newParam->ParamType = JSON_PARAM_TYPE_UINT32;
	newParam->ParamValue.Uint32Param = ParamValue;
	newParam->Next = 0;

	uint32 nameLength = strlen(ParamName) + 1;
    iStatus = CFE_ES_GetPoolBuf((uint32 **)&(newParam->ParamName), Parent->HandlePtr, nameLength);
    if(iStatus <= 0)
    {
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)Parent->End);
    	return 0;
    }
    strcpy(newParam->ParamName, ParamName);

    return CFE_SUCCESS;
}



int32 JSON_AddUint16(JSON_Object *Parent, const char *ParamName, uint16 ParamValue)
{
	return -1;
}



int32 JSON_AddUint8(JSON_Object *Parent, const char *ParamName, uint8 ParamValue)
{
	return -1;
}



int32 JSON_AddInt32(JSON_Object *Parent, const char *ParamName, int32 ParamValue)
{
	return -1;
}



int32 JSON_AddInt16(JSON_Object *Parent, const char *ParamName, int16 ParamValue)
{
	return -1;
}



int32 JSON_AddInt8(JSON_Object *Parent, const char *ParamName, int8 ParamValue)
{
	return -1;
}



int32 JSON_AddBoolean(JSON_Object *Parent, const char *ParamName, boolean ParamValue)
{
	return -1;
}



int32 JSON_AddArray(JSON_Object *Parent, const char *ParamName, JSON_Object** NewObject)
{
	JSON_Object *jsonObj = 0;
	JSON_AddObject(Parent, ParamName, NewObject);
	jsonObj = *NewObject;
	jsonObj->IsArray = TRUE;
}



int32 JSON_AddObject(JSON_Object *Parent, const char *ParamName, JSON_Object** NewObject)
{
	int32 iStatus = CFE_SUCCESS;

	JSON_Param *newParam;

	if(Parent->Start == 0)
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->Start, Parent->HandlePtr,  sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }

	    Parent->End = Parent->Start;
	}
	else
	{
	    iStatus = CFE_ES_GetPoolBuf((uint32 **)&Parent->End->Next, Parent->HandlePtr, sizeof(JSON_Param));
	    if(iStatus <= 0)
	    {
	    	return 0;
	    }
	    Parent->End = Parent->End->Next;
	}

	newParam = Parent->End;

	newParam->ParamType = JSON_PARAM_TYPE_OBJECT;

	JSON_Alloc(&newParam->ParamValue.ObjectParam, Parent->HandlePtr);
	newParam->ParamValue.ObjectParam->Parent = Parent;
	newParam->Next = 0;

	uint32 nameLength = strlen(ParamName) + 1;
    iStatus = CFE_ES_GetPoolBuf((uint32 **)&(newParam->ParamName), Parent->HandlePtr, nameLength);
    if(iStatus <= 0)
    {
    	CFE_ES_PutPoolBuf(Parent->HandlePtr, (uint32 *)Parent->End);
    	return 0;
    }
    strcpy(newParam->ParamName, ParamName);

    *NewObject = newParam->ParamValue.ObjectParam;

    return CFE_SUCCESS;
}



int32 JSON_GetText(JSON_Object *JsonObject, char *OutputBuffer, uint32 OutputSize, boolean Append)
{
	JSON_Param *jsonParam = JsonObject->Start;

	if(JsonObject->IsArray)
	{
		if(Append)
		{
			strncat(OutputBuffer, "[", OutputSize);
		}
		else
		{
			strncpy(OutputBuffer, "[", OutputSize);
		}
	}
	else
	{
		if(Append)
		{
			strncat(OutputBuffer, "{\n", OutputSize);
		}
		else
		{
			strncpy(OutputBuffer, "{\n", OutputSize);
		}
	}

	while(jsonParam != 0)
	{
		uint32 len = strlen(OutputBuffer);
		switch(jsonParam->ParamType)
		{
			case JSON_PARAM_TYPE_STRING:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\"", jsonParam->ParamValue.StringParam);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":\"%s\"", jsonParam->ParamName, jsonParam->ParamValue.StringParam);
				}
				break;

			case JSON_PARAM_TYPE_FLOAT:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%f", jsonParam->ParamValue.FloatParam);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%f", jsonParam->ParamName, jsonParam->ParamValue.FloatParam);
				}
				break;

			case JSON_PARAM_TYPE_UINT32:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%u", jsonParam->ParamValue.Uint32Param);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%u", jsonParam->ParamName, jsonParam->ParamValue.Uint32Param);
				}
				break;

			case JSON_PARAM_TYPE_UINT16:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%u", jsonParam->ParamValue.Uint16Param);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%u", jsonParam->ParamName, jsonParam->ParamValue.Uint16Param);
				}
				break;

			case JSON_PARAM_TYPE_UINT8:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%u", jsonParam->ParamValue.Uint8Param);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%u", jsonParam->ParamName, jsonParam->ParamValue.Uint8Param);
				}
				break;

			case JSON_PARAM_TYPE_INT32:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%i", jsonParam->ParamValue.Uint32Param);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%i", jsonParam->ParamName, jsonParam->ParamValue.Uint32Param);
				}
				break;

			case JSON_PARAM_TYPE_INT16:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%i", jsonParam->ParamValue.Uint16Param);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%i", jsonParam->ParamName, jsonParam->ParamValue.Uint16Param);
				}
				break;

			case JSON_PARAM_TYPE_INT8:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%i", jsonParam->ParamValue.Uint8Param);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%i", jsonParam->ParamName, jsonParam->ParamValue.Uint8Param);
				}
				break;

			case JSON_PARAM_TYPE_BOOL:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%s", (jsonParam->ParamValue.BoolParam ? "true": "false" ));
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%s", jsonParam->ParamName, (jsonParam->ParamValue.BoolParam ? "true": "false" ));
				}
				break;

			case JSON_PARAM_TYPE_OBJECT:
			case JSON_PARAM_TYPE_ARRAY:
			/* Fallthru */
			{
				if(JsonObject->IsArray)
				{
					JSON_GetText(jsonParam->ParamValue.ObjectParam, OutputBuffer + len, OutputSize - len, TRUE);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":", jsonParam->ParamName);
					JSON_GetText(jsonParam->ParamValue.ObjectParam, OutputBuffer + len, OutputSize - len, TRUE);
				}
				break;
			}

			case JSON_PARAM_TYPE_NUMBER:
				if(JsonObject->IsArray)
				{
					snprintf(OutputBuffer + len, OutputSize - len, "%s", jsonParam->ParamValue.StringParam);
				}
				else
				{
					snprintf(OutputBuffer + len, OutputSize - len, "\"%s\":%s", jsonParam->ParamName, jsonParam->ParamValue.StringParam);
				}
				break;

			default:
				/* TODO */
				break;
		}

		jsonParam = jsonParam->Next;
		if(jsonParam != 0)
		{
			strncat(OutputBuffer, ",\n", OutputSize);
		}
	}

	if(JsonObject->IsArray)
	{
		strncat(OutputBuffer, "]", OutputSize);
	}
	else
	{
		strncat(OutputBuffer, "}", OutputSize);
	}

	return CFE_SUCCESS;
}



int32 JSON_ResetParser(JSON_ParserData *ParserData, JSON_Object *Parent)
{
	ParserData->State = JSON_PARSER_UNKNOWN;
	ParserData->Parent = Parent;
	ParserData->ParamName[0] = '\0';
	ParserData->ParamNameCursor = 0;
	ParserData->ParamValue[0] = '\0';
	ParserData->ParamValueCursor = 0;

	return CFE_SUCCESS;
}



int32 JSON_ParseChar(JSON_ParserData *ParserData, JSON_Object *Parent, char InputChar)
{
	switch(ParserData->State)
	{
		case JSON_PARSER_UNKNOWN:
		{
			switch(InputChar)
			{
				case '{':
				{
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					ParserData->Parent = Parent;
					ParserData->ParamName[0] = '\0';
					ParserData->ParamNameCursor = 0;
					break;
				}

				default:
				{
					break;

				}
			}
			break;
		}

		case JSON_PARSER_LOOKING_FOR_PARAM_NAME:
		{
			switch(InputChar)
			{
				case '"':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_NAME;
					ParserData->ParamNameCursor = 0;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_NAME:
		{
			switch(InputChar)
			{
				case '"':
				{
					ParserData->ParamName[ParserData->ParamNameCursor] = '\0';
					ParserData->State = JSON_PARSER_LOOKING_FOR_COLON;
					break;
				}

				default:
				{
					ParserData->ParamName[ParserData->ParamNameCursor] = InputChar;
					ParserData->ParamNameCursor++;
					break;
				}
			}
			break;
		}

		case JSON_PARSER_LOOKING_FOR_COLON:
		{
			switch(InputChar)
			{
				case ':':
				{
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					ParserData->ParamName[ParserData->ParamNameCursor] = '\0';
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_LOOKING_FOR_PARAM_VALUE:
		{
			switch(InputChar)
			{
				case '"':
				{
					ParserData->ParamValueCursor = 0;
					ParserData->State = JSON_PARSER_PARSING_PARAM_STRING;
					break;
				}

				case '{':
				{
					JSON_Object *jsonNewObj = 0;
					JSON_AddObject(ParserData->Parent, ParserData->ParamName, &jsonNewObj);
					ParserData->Parent = jsonNewObj;
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					break;
				}

				case '[':
				{
					JSON_Object *jsonNewObj = 0;
					JSON_AddArray(ParserData->Parent, ParserData->ParamName, &jsonNewObj);
					ParserData->Parent = jsonNewObj;
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					break;
				}

				case ']':
				{
					ParserData->Parent = ParserData->Parent->Parent;
					ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					break;
				}

				case 'T':
				case 't':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_TRUE_1;
					break;
				}

				case 'F':
				case 'f':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_FALSE_1;
					break;
				}

				case '\t':
				case ' ':
				case '\n':
				case '\r':
					/* This is white space.  Ignore it. */
					break;

				case '.':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_FLOAT;
					ParserData->ParamValue[ParserData->ParamValueCursor] = InputChar;
					ParserData->ParamValueCursor++;
					break;
				}

				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					ParserData->ParamValueCursor = 0;
					ParserData->ParamValue[ParserData->ParamValueCursor] = InputChar;
					ParserData->ParamValueCursor++;
					ParserData->State = JSON_PARSER_PARSING_PARAM_NUMBER;
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_TRUE_1:
		{
			switch(InputChar)
			{
				case 'R':
				case 'r':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_TRUE_2;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_TRUE_2:
		{
			switch(InputChar)
			{
				case 'U':
				case 'u':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_TRUE_3;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_TRUE_3:
		{
			switch(InputChar)
			{
				case 'E':
				case 'e':
				{
					/* The value "TRUE" was found.  Add a boolean object and
					 * start looking for the command delineating the
					 * parameter.
					 */
					if(ParserData->Parent->IsArray == TRUE)
					{
						JSON_AddBoolean(ParserData->Parent, "", TRUE);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					}
					else
					{
						JSON_AddBoolean(ParserData->Parent, ParserData->ParamName, TRUE);
						ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					}
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_FALSE_1:
		{
			switch(InputChar)
			{
				case 'A':
				case 'a':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_FALSE_2;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_FALSE_2:
		{
			switch(InputChar)
			{
				case 'L':
				case 'l':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_FALSE_3;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_FALSE_3:
		{
			switch(InputChar)
			{
				case 'S':
				case 's':
				{
					ParserData->State = JSON_PARSER_PARSING_PARAM_FALSE_4;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_FALSE_4:
		{
			switch(InputChar)
			{
				case 'E':
				case 'e':
				{
					/* The value "FALSE" was found.  Add a boolean object and
					 * start looking for the command delineating the
					 * parameter.
					 */
					if(ParserData->Parent->IsArray == TRUE)
					{
						JSON_AddBoolean(ParserData->Parent, "", FALSE);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					}
					else
					{
						JSON_AddBoolean(ParserData->Parent, ParserData->ParamName, FALSE);
						ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					}
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_NUMBER:
		{
			switch(InputChar)
			{
				case '.':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = InputChar;
					ParserData->ParamValueCursor++;
					ParserData->State = JSON_PARSER_PARSING_PARAM_FLOAT;
					break;
				}

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = InputChar;
					ParserData->ParamValueCursor++;
					ParserData->State = JSON_PARSER_PARSING_PARAM_NUMBER;
					break;
				}

				case ',':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					if(ParserData->Parent->IsArray == TRUE)
					{
						JSON_AddNumber(ParserData->Parent, "", ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					}
					else
					{
						JSON_AddNumber(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					}
					break;
				}

				case ']':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					JSON_AddNumber(ParserData->Parent, "", ParserData->ParamValue);
					ParserData->Parent = ParserData->Parent->Parent;
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					break;
				}

				case '}':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					JSON_AddNumber(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
					ParserData->Parent = ParserData->Parent->Parent;
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					break;
				}

				case '\t':
				case ' ':
				case '\n':
				case '\r':
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					if(ParserData->Parent->IsArray == TRUE)
					{
						JSON_AddNumber(ParserData->Parent, "", ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					}
					else
					{
						JSON_AddNumber(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					}
					break;

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_FLOAT:
		{
			switch(InputChar)
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = InputChar;
					ParserData->ParamValueCursor++;
					ParserData->State = JSON_PARSER_PARSING_PARAM_NUMBER;
					break;
				}

				case ',':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					JSON_AddNumber(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					break;
				}

				case '}':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					JSON_AddNumber(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
					ParserData->Parent = ParserData->Parent->Parent;
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					break;
				}

				case '\t':
				case ' ':
				case '\n':
				case '\r':
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					if(ParserData->Parent->IsArray == TRUE)
					{
						JSON_AddNumber(ParserData->Parent, "", ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					}
					else
					{
						JSON_AddNumber(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					}
					break;

				default:
				{
					/* TODO */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_PARSING_PARAM_STRING:
		{
			switch(InputChar)
			{
				case '"':
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = '\0';
					if(ParserData->Parent->IsArray == TRUE)
					{
						JSON_AddString(ParserData->Parent, "", ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_VALUE;
					}
					else
					{
						JSON_AddString(ParserData->Parent, ParserData->ParamName, ParserData->ParamValue);
						ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					}
					break;
				}

				default:
				{
					ParserData->ParamValue[ParserData->ParamValueCursor] = InputChar;
					ParserData->ParamValueCursor++;
					/* TODO - Add escape sequencing. */
					break;
				}
			}
			break;
		}

		case JSON_PARSER_LOOKING_FOR_COMMA:
		{
			switch(InputChar)
			{
				case ',':
				{
					ParserData->State = JSON_PARSER_LOOKING_FOR_PARAM_NAME;
					break;
				}

				case '}':
				{
					ParserData->Parent = ParserData->Parent->Parent;
					ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					break;
				}

				case ']':
				{
					ParserData->Parent = ParserData->Parent->Parent;
					ParserData->State = JSON_PARSER_LOOKING_FOR_COMMA;
					break;
				}

				default:
				{
					/* TODO */
					break;
				}
			}

			break;
		}

		default:
		{
			/* TODO */
			break;
		}
	}
	return 0;
}



int32 JSON_GetParam(JSON_Object *Parent, const char *ParamName, JSON_Param **Param)
{
	int32 iStatus = 0;

	JSON_Param *tmpParam = *Param;

	tmpParam = Parent->Start;

	while(tmpParam)
	{
		if(strncmp(ParamName, tmpParam->ParamName, JSON_PARAM_NAME_SIZE) == 0)
		{
			*Param = tmpParam;
			return CFE_SUCCESS;
		}

		tmpParam = tmpParam->Next;
	}

	return -1;
}



int32 JSON_GetString(JSON_Object *Parent, const char *ParamName, char **ParamValue)
{
	JSON_Param *param = 0;

	JSON_GetParam(Parent, ParamName, &param);

	if(param == 0)
	{
		return -1;
	}

	*ParamValue = param->ParamValue.StringParam;

	return CFE_SUCCESS;
}



int32 JSON_GetFloat(JSON_Object *Parent, const char *ParamName, float *ParamValue)
{

}



int32 JSON_GetUint32(JSON_Object *Parent, const char *ParamName, uint32 *ParamValue)
{
	JSON_Param *param = 0;

	JSON_GetParam(Parent, ParamName, &param);

	if(param == 0)
	{
		return -1;
	}

	if(sscanf(param->ParamValue.StringParam, "%u", (unsigned int *)ParamValue) == EOF)
	{
		return -1;
	}

	return CFE_SUCCESS;
}



int32 JSON_GetUint16(JSON_Object *Parent, const char *ParamName, uint16 *ParamValue)
{

}



int32 JSON_GetUint8(JSON_Object *Parent, const char *ParamName, uint8 *ParamValue)
{

}



int32 JSON_GetInt32(JSON_Object *Parent, const char *ParamName, int32 *ParamValue)
{

}



int32 JSON_GetInt16(JSON_Object *Parent, const char *ParamName, int16 *ParamValue)
{

}



int32 JSON_GetInt8(JSON_Object *Parent, const char *ParamName, int8 *ParamValue)
{

}



int32 JSON_GetBoolean(JSON_Object *Parent, const char *ParamName, boolean *ParamValue)
{

}



int32 JSON_GetArray(JSON_Object *Parent, const char *ParamName, JSON_Object* JsonObject)
{

}



int32 JSON_GetObject(JSON_Object *Parent, const char *ParamName, JSON_Object* JsonObject)
{

}





