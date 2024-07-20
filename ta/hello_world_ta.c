/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <hello_world_ta.h>
#include <string.h>

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */

#define BASE64_ENCODED_SIZE(len) (((len + 2) / 3) * 4 + 1)
#define BASE64_DECODED_SIZE(len) (((len) / 4) * 3)


static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t Base64decode(char *decoded, const char *data, size_t input_length) {
    size_t output_length = BASE64_DECODED_SIZE(input_length);
    size_t i, j;
    uint32_t sextet_a, sextet_b, sextet_c, sextet_d;
    uint32_t triple;

    for (i = 0, j = 0; i < input_length;) {
        sextet_a = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        sextet_b = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        sextet_c = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        sextet_d = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;

        triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < output_length) decoded[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) decoded[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) decoded[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return j; // Return the actual length of the decoded data
}

// Base64 encoding table

// Function to encode data to Base64
size_t Base64encode(char *encoded, const char *data, size_t input_length) {
    size_t output_length = 4 * ((input_length + 2) / 3);
    size_t i, j;

    for (i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded[j++] = base64_table[(triple >> 3 * 6) & 0x3F];
        encoded[j++] = base64_table[(triple >> 2 * 6) & 0x3F];
        encoded[j++] = base64_table[(triple >> 1 * 6) & 0x3F];
        encoded[j++] = base64_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < (3 - (input_length % 3)) % 3; i++) {
        encoded[output_length - 1 - i] = '=';
    }

    encoded[output_length] = '\0';  // Null-terminate the encoded string

    return output_length;
}


TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}
   
/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called??????");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Hello World HW4!\n");
	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

static TEE_Result inc_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	params[0].value.a++;
	IMSG("Increase value to: %u", params[0].value.a);

	return TEE_SUCCESS;
}

// static TEE_Result encode(uint32_t param_types, TEE_Param params[4]) {
//        uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
// 	IMSG("-------------------------------------------\n");
//     if (param_types != exp_param_types) {
//         IMSG("Bad parameters: expected 0x%x, got 0x%x", exp_param_types, param_types);
//         return TEE_ERROR_BAD_PARAMETERS;
//     }

//     // Get the input string
//     char *input_string = (char *)params[0].memref.buffer;
//     size_t input_size = params[0].memref.size;

//     // Get the output buffer
//     char *output_string = (char *)params[1].memref.buffer;
//     size_t output_size = params[1].memref.size;

//     // Example: Just copy the input string to the output buffer
//     if (output_size < input_size) {
//         IMSG("Output buffer too short: expected %zu, got %zu", input_size, output_size);
//         return TEE_ERROR_SHORT_BUFFER;
//     }

//     TEE_MemMove(output_string, input_string, input_size);

//     return TEE_SUCCESS;

// }
static TEE_Result encode(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        EMSG("Bad parameters: expected 0x%x, got 0x%x", exp_param_types, param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    // Get the input string
    char *input_string = (char *)params[0].memref.buffer;
    size_t input_size = params[0].memref.size;

    // Get the output buffer
    char *output_string = (char *)params[1].memref.buffer;
    size_t output_size = params[1].memref.size;

    // Calculate the required buffer size for the Base64 encoded string
    size_t required_size = BASE64_ENCODED_SIZE(input_size);

    if (output_size < required_size) {
        EMSG("Output buffer too short: expected %zu, got %zu", required_size, output_size);
        return TEE_ERROR_SHORT_BUFFER;
    }

    IMSG("Encoding input string to Base64");
    size_t encoded_length = Base64encode(output_string, input_string, input_size);

    if (encoded_length == 0) {
        EMSG("Base64 encoding failed");
        return TEE_ERROR_GENERIC;
    }

    return TEE_SUCCESS;
}

static TEE_Result decode(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        EMSG("Bad parameters: expected 0x%x, got 0x%x", exp_param_types, param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    // Get the input string
    char *input_string = (char *)params[0].memref.buffer;
    size_t input_size = params[0].memref.size;

    // Get the output buffer
    char *output_string = (char *)params[1].memref.buffer;
    size_t output_size = params[1].memref.size;

    // Calculate the required buffer size for the Base64 decoded string
    size_t required_size = BASE64_DECODED_SIZE(input_size);

    if (output_size < required_size) {
        EMSG("Output buffer too short: expected %zu, got %zu", required_size, output_size);
        return TEE_ERROR_SHORT_BUFFER;
    }

    IMSG("Decoding input string from Base64");
    size_t decoded_length = Base64decode(output_string, input_string, input_size);

    if (decoded_length == 0) {
        EMSG("Base64 decoding failed");
        return TEE_ERROR_GENERIC;
    }

    return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_HELLO_WORLD_CMD_INC_VALUE:
		return inc_value(param_types, params);
	case TA_ENCODE:
		return encode(param_types, params);
	case TA_DECODE:
		return decode(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
