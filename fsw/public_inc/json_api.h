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

#ifndef _JSON_API_H_
#define _JSON_API_H_

#include "cfe.h"

#define JSON_PARAM_NAME_SIZE	255
#define JSON_PARAM_VALUE_SIZE	255

typedef struct JSON_Object
{
	struct JSON_Param*	Start;
	struct JSON_Param*	End;
	CFE_ES_MemHandle_t *HandlePtr;
	boolean 			IsArray;
	struct JSON_Object	*Parent;
} JSON_Object;

typedef union
{
	char 			*StringParam;
	float			FloatParam;
	uint32			Uint32Param;
	uint16  		Uint16Param;
	uint8			Uint8Param;
	int32			Int32Param;
	int16			Int16Param;
	int8			Int8Param;
	boolean			BoolParam;
	JSON_Object*	ObjectParam;
	JSON_Object*	ArrayParam;
} JSON_ParamValue;

typedef enum
{
	JSON_PARAM_TYPE_UNKNOWN = 0,
	JSON_PARAM_TYPE_STRING,
	JSON_PARAM_TYPE_FLOAT,
	JSON_PARAM_TYPE_UINT32,
	JSON_PARAM_TYPE_UINT16,
	JSON_PARAM_TYPE_UINT8,
	JSON_PARAM_TYPE_INT32,
	JSON_PARAM_TYPE_INT16,
	JSON_PARAM_TYPE_INT8,
	JSON_PARAM_TYPE_BOOL,
	JSON_PARAM_TYPE_OBJECT,
	JSON_PARAM_TYPE_ARRAY,
	JSON_PARAM_TYPE_NUMBER
} JSON_ParamType;

typedef struct JSON_Param
{
	char				*ParamName;
	JSON_ParamType		ParamType;
	JSON_ParamValue 	ParamValue;
	struct JSON_Param	*Next;
} JSON_Param;

typedef enum
{
	JSON_PARSER_UNKNOWN,
	JSON_PARSER_LOOKING_FOR_PARAM_NAME,
	JSON_PARSER_PARSING_PARAM_NAME,
	JSON_PARSER_LOOKING_FOR_COLON,
	JSON_PARSER_LOOKING_FOR_PARAM_VALUE,
	JSON_PARSER_PARSING_PARAM_TRUE_1,
	JSON_PARSER_PARSING_PARAM_TRUE_2,
	JSON_PARSER_PARSING_PARAM_TRUE_3,
	JSON_PARSER_PARSING_PARAM_FALSE_1,
	JSON_PARSER_PARSING_PARAM_FALSE_2,
	JSON_PARSER_PARSING_PARAM_FALSE_3,
	JSON_PARSER_PARSING_PARAM_FALSE_4,
	JSON_PARSER_PARSING_PARAM_NUMBER,
	JSON_PARSER_PARSING_PARAM_FLOAT,
	JSON_PARSER_PARSING_PARAM_STRING,
	JSON_PARSER_LOOKING_FOR_COMMA
} JSON_ParserState;

typedef struct
{
	JSON_ParserState	State;
	char				ParamName[JSON_PARAM_NAME_SIZE];
	uint32				ParamNameCursor;
	char				ParamValue[JSON_PARAM_VALUE_SIZE];
	uint32				ParamValueCursor;
	boolean				IsNegative;
	JSON_Object 		*Parent;
} JSON_ParserData;

int32 JSON_Alloc(JSON_Object** JsonObject, CFE_ES_MemHandle_t *HandlePtr);
int32 JSON_Dealloc(JSON_Object* JsonObject);
int32 JSON_AddString(JSON_Object *Parent, const char *ParamName, const char *ParamValue);
int32 JSON_AddFloat(JSON_Object *Parent, const char *ParamName, float ParamValue);
int32 JSON_AddUint32(JSON_Object *Parent, const char *ParamName, uint32 ParamValue);
int32 JSON_AddUint16(JSON_Object *Parent, const char *ParamName, uint16 ParamValue);
int32 JSON_AddUint8(JSON_Object *Parent, const char *ParamName, uint8 ParamValue);
int32 JSON_AddInt32(JSON_Object *Parent, const char *ParamName, int32 ParamValue);
int32 JSON_AddInt16(JSON_Object *Parent, const char *ParamName, int16 ParamValue);
int32 JSON_AddInt8(JSON_Object *Parent, const char *ParamName, int8 ParamValue);
int32 JSON_AddBoolean(JSON_Object *Parent, const char *ParamName, boolean ParamValue);
int32 JSON_AddArray(JSON_Object *Parent, const char *ParamName, JSON_Object** JsonObject);
int32 JSON_AddObject(JSON_Object *Parent, const char *ParamName, JSON_Object** JsonObject);
int32 JSON_AddNumber(JSON_Object *Parent, const char *ParamName, const char *ParamValue);
int32 JSON_GetText(JSON_Object *JsonObject, char *OutputBuffer, uint32 Size, boolean Append);

int32 JSON_ParseChar(JSON_ParserData *ParserData, JSON_Object *Parent, char InputChar);
int32 JSON_ResetParser(JSON_ParserData *ParserData, JSON_Object *Parent);


int32 JSON_GetString(JSON_Object *Parent, const char *ParamName, char **ParamValue);
int32 JSON_GetFloat(JSON_Object *Parent, const char *ParamName, float *ParamValue);
int32 JSON_GetUint32(JSON_Object *Parent, const char *ParamName, uint32 *ParamValue);
int32 JSON_GetUint16(JSON_Object *Parent, const char *ParamName, uint16 *ParamValue);
int32 JSON_GetUint8(JSON_Object *Parent, const char *ParamName, uint8 *ParamValue);
int32 JSON_GetInt32(JSON_Object *Parent, const char *ParamName, int32 *ParamValue);
int32 JSON_GetInt16(JSON_Object *Parent, const char *ParamName, int16 *ParamValue);
int32 JSON_GetInt8(JSON_Object *Parent, const char *ParamName, int8 *ParamValue);
int32 JSON_GetBoolean(JSON_Object *Parent, const char *ParamName, boolean *ParamValue);
int32 JSON_GetArray(JSON_Object *Parent, const char *ParamName, JSON_Object* JsonObject);
int32 JSON_GetObject(JSON_Object *Parent, const char *ParamName, JSON_Object* JsonObject);


#endif
