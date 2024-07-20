#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>
#include <hello_world_ta.h>

/* For the UUID (found in the TA's h-file(s)) */
int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_Result res;
	TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
    uint32_t err_origin;

    // Initialize context
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed with code 0x%x\n", res);
        return -1;
    }

    // Open session
		res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed with code 0x%x origin 0x%x\n", res, err_origin);
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

     if (strcmp(argv[1], "--encode") == 0) {
        memset(&op, 0, sizeof(op));
        char input_string[127];
        char output_string[127];  // Allocate space for the output string
        printf("Input:");
        scanf("%s", input_string);
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
        op.params[0].tmpref.buffer = input_string;
        op.params[0].tmpref.size = strlen(input_string) + 1;
        op.params[1].tmpref.buffer = output_string;
        op.params[1].tmpref.size = sizeof(output_string);

        // Invoke the command
        res = TEEC_InvokeCommand(&sess, TA_ENCODE, &op, &err_origin);
        if (res != TEEC_SUCCESS) {
            printf("Error with 0x%x origin 0x%x\n", res, err_origin);
        } else {
            printf("Output from TA: %s\n", output_string);
        }
    } else if (strcmp(argv[1], "--decode") == 0) {
        char input_string[127]; // "Hello, World!" in Base64
        char output_string[127]; // Buffer to hold the decoded string
        printf("Input:");
        scanf("%s", input_string);
        // Clear the operation structure and set parameters
        memset(&op, 0, sizeof(op));
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
        op.params[0].tmpref.buffer = input_string;
        op.params[0].tmpref.size = strlen(input_string);
        op.params[1].tmpref.buffer = output_string;
        op.params[1].tmpref.size = 256;

        // Invoke the command to decode the Base64 string
        res = TEEC_InvokeCommand(&sess, TA_DECODE, &op, &err_origin);
        if (res != TEEC_SUCCESS) {
            printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, err_origin);
        } else {
            printf("Decoded string: %s\n", output_string);
        }
    } else {
        printf("Invalid argument. Use --encode or --decode.\n");
    }





    // Prepare the operation
    // memset(&op, 0, sizeof(op));

	// op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
	// 				 TEEC_NONE, TEEC_NONE);
	// op.params[0].value.a = 42;

    // char input_string[] = "王大明";
    // char output_string[100];  // Allocate space for the output string

    // op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    // op.params[0].tmpref.buffer = input_string;
    // op.params[0].tmpref.size = strlen(input_string) + 1;
    // op.params[1].tmpref.buffer = output_string;
    // op.params[1].tmpref.size = sizeof(output_string);

    // // Invoke the command
    // res = TEEC_InvokeCommand(&sess, TA_ENCODE, &op, &err_origin);
    // if (res != TEEC_SUCCESS) {
    //     printf("Error with 0x%x origin 0x%x\n", res, err_origin);
    // } else {
    //     printf("Output from TA: %s\n", output_string);
    // }
    //546L5aSn5piOAA==
    //---------------------------------
    // char input_string[] = "546L5aSn5piOAA=="; // "Hello, World!" in Base64
    // char output_string[256]; // Buffer to hold the decoded string

    // // Clear the operation structure and set parameters
    // memset(&op, 0, sizeof(op));
    // op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    // op.params[0].tmpref.buffer = input_string;
    // op.params[0].tmpref.size = strlen(input_string);
    // op.params[1].tmpref.buffer = output_string;
    // op.params[1].tmpref.size = 256;

    // // Invoke the command to decode the Base64 string
    // res = TEEC_InvokeCommand(&sess, TA_DECODE, &op, &err_origin);
    // if (res != TEEC_SUCCESS) {
    //     printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, err_origin);
    // } else {
    //     printf("Decoded string: %s\n", output_string);
    // }


    // Close the session and finalize context
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}
