#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <hello_world_ta.h>
int main() {
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

    // Prepare the operation
    memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 42;

    char input_string[] = "王大明";
    char output_string[100];  // Allocate space for the output string

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = input_string;
    op.params[0].tmpref.size = strlen(input_string) + 1;
    op.params[1].tmpref.buffer = output_string;
    op.params[1].tmpref.size = sizeof(output_string);

    // Invoke the command
    res = TEEC_InvokeCommand(&sess, TA_ENCODE, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        printf("sakd 0x%x origin 0x%x\n", res, err_origin);
    } else {
        printf("Output from TA: %s\n", output_string);
    }

    // Close the session and finalize context
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}
