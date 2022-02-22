#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define lockScriptLen 48
#define signatureLen 48
#define tx_id_len 32

struct Output{
    int amt;
    char public_key_hash[lockScriptLen];
};
typedef struct Output Output;

struct Input{
    char signature[signatureLen];
    char prev_tx_id[tx_id_len];
    int prev_utxo_output;
};
typedef struct Input Input;

struct Transaction{
    int num_inputs;
    int num_outputs;
    Input *inputs;
    Output *outputs;
};

typedef struct Transaction Transaction;

char * ser_Tx(Transaction *tx){
    // Determine total Size in bytes
    // This might be bad...https://stackoverflow.com/questions/119123/why-isnt-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member?rq=1
    int total_size = (sizeof(tx->num_inputs)+sizeof(tx->num_outputs) + 
        tx->num_inputs*sizeof(Input) + tx->num_outputs*sizeof(Output)+sizeof(char));
    
    printf("Size Transaction (Bytes): %i\n", total_size);
    //Pack it all in
    char * data = malloc(total_size);

    int offset = 0;
    memcpy(data, &(tx->num_inputs), sizeof(tx->num_inputs));

    char * num_outputs = data+sizeof(tx->num_inputs);
    memcpy(num_outputs, &(tx->num_outputs), sizeof(tx->num_outputs));
    // int test;
    // memcpy(&test, data, sizeof(tx->num_inputs));
    // printf("recovered int: %i\n", test);

    char * inputs = num_outputs+sizeof(tx->num_outputs);
    memcpy(inputs, tx->inputs, tx->num_inputs*sizeof(Input));

    char * outputs = inputs + tx->num_inputs*sizeof(Input);
    memcpy(outputs, tx->outputs, tx->num_outputs*sizeof(Output));

    char * terminate = outputs + tx->num_outputs*sizeof(Output);
    terminate = '\0';
    // Try to visualize if the data is there
    printf("serilalized: %s\n" ,data);
    fwrite (data , sizeof(char), total_size, stdout);
    return data;
}

Transaction* deser_tx(char *data){
    Transaction * new_tx = malloc(sizeof(Transaction));
    

    memcpy(&(new_tx->num_inputs), data, sizeof(int));

    char * nm_outputs = data+sizeof(int);
    memcpy(&(new_tx->num_outputs), nm_outputs, sizeof(int));

    char * inputs = nm_outputs+sizeof(int);
    int input_sz = new_tx->num_inputs*sizeof(Input);
    new_tx->inputs = malloc(input_sz);
    memcpy(new_tx->inputs, inputs, input_sz);

    char * outputs = inputs+input_sz;
    int output_sz = new_tx->num_outputs*sizeof(Output);
    new_tx->outputs = malloc(output_sz);
    memcpy(new_tx->outputs, outputs, output_sz);

    
    printf("\nnum_inputs: %i\n", new_tx->num_inputs);
    printf("num_outputs: %i\n", new_tx->num_outputs);
    printf("prev_tx_id: %s\n", new_tx->inputs->prev_tx_id);
    printf("prev_tx_id: %s\n", new_tx->inputs[1].prev_tx_id);
    printf("Output Amount: %i\n", new_tx->outputs[0].amt);
    printf("Output PubKey: %s\n", new_tx->outputs[0].public_key_hash);
}

int main() {

    Output *an_Output = malloc(sizeof(Output));

    an_Output->amt = 5;
    strcpy(an_Output->public_key_hash, "a_val");

    printf("amt: %i\n", an_Output->amt);
    printf("private Key: %s\n", an_Output->public_key_hash);
    printf("Output(Bytes): %li\n", sizeof(Output));



    Input *an_Input = malloc(sizeof(Input));

    an_Input->prev_utxo_output = 2;
    strcpy(an_Input->signature, "signature");
    strcpy(an_Input->prev_tx_id, "prev_tx_id");

    printf("\nINPUTS!\n");
    printf("sig: %s\n", an_Input->signature);
    printf("prev_tx_id: %s\n", an_Input->prev_tx_id);
    printf("prev_output_num: %i\n", an_Input->prev_utxo_output);
    printf("Input(Bytes): %li\n", sizeof(Input));


    Transaction *a_Tx = malloc(sizeof(Transaction));
    a_Tx->num_inputs = 2;
    a_Tx->num_outputs = 1;

    a_Tx->inputs = malloc(sizeof(Input)*(a_Tx->num_inputs));
    a_Tx->outputs = malloc(sizeof(Output)*(a_Tx->num_outputs));

    // Copy over some inputs
    memcpy(a_Tx->inputs, an_Input, sizeof(*an_Input) );
    memcpy(a_Tx->inputs+1, an_Input, sizeof(*an_Input) );

    //Copy an Output
    memcpy(a_Tx->outputs, an_Output, sizeof(*an_Output) );

    printf("\nnum_inputs: %i\n", a_Tx->num_inputs);
    printf("num_outputs: %i\n", a_Tx->num_outputs);


    printf("prev_tx_id[0]: %s\n", a_Tx->inputs->prev_tx_id);
    printf("prev_tx_id[1]: %s\n", a_Tx->inputs[1].prev_tx_id);

    printf("Output Amount: %i\n", a_Tx->outputs[0].amt);
    printf("Output PubKey: %s\n", a_Tx->outputs[0].public_key_hash);
    //printf("prev_tx_id: %s\n", ((a_Tx->inputs)+1)->prev_tx_id);


    printf("Tx(Bytes): %li\n", sizeof(Transaction));


    //Serialization Testing
    char* char_tx = ser_Tx(a_Tx);

    Transaction * other_tx = deser_tx(char_tx);
    return 0;
}

