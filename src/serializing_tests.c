#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define lockScriptLen 48
#define signatureLen 48
#define tx_id_len 32

struct Output{
    unsigned long amt;
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

struct UTXO{
    unsigned long amt;
    char signature[signatureLen];
    short spent;
};
typedef struct UTXO UTXO;

char * ser_UTXO(UTXO *utxo){
    char * data = malloc(sizeof(UTXO));
    memcpy(data, &(utxo->amt), sizeof(utxo->amt));

    char * sig = data+sizeof(utxo->amt);
    memcpy(sig, &(utxo->signature), sizeof(utxo->signature));

    char * spent = sig+sizeof(utxo->signature);
    memcpy(spent, &(utxo->spent), sizeof(utxo->spent));

    return data;
}

UTXO * dser_UTXO(char * data){
    UTXO * new_UTXO = malloc(sizeof(UTXO));

    // Stolen from https://cboard.cprogramming.com/cplusplus-programming/43180-sizeof-struct-member-problem.html
    memcpy(&(new_UTXO->amt), data, sizeof(((UTXO*)0)->amt)); 

    char * sig = data + sizeof(((UTXO*)0)->amt);
    memcpy(&(new_UTXO->signature), sig, sizeof(((UTXO*)0)->signature));

    char * spent = sig + sizeof(((UTXO*)0)->signature);
    memcpy(&(new_UTXO->spent), spent, sizeof(((UTXO*)0)->spent));

    return new_UTXO;
}

char * ser_Tx(Transaction *tx){
    // Determine total Size in bytes
    // This might be bad...https://stackoverflow.com/questions/119123/why-isnt-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member?rq=1
    int total_size = (sizeof(tx->num_inputs)+sizeof(tx->num_outputs) + 
        tx->num_inputs*sizeof(Input) + tx->num_outputs*sizeof(Output)+sizeof(char));
    
    printf("Size Transaction (Bytes): %i\n", total_size);
    //Pack it all in
    char * data = malloc(total_size);

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
    printf("Output Amount: %li\n", new_tx->outputs[0].amt);
    printf("Output PubKey: %s\n", new_tx->outputs[0].public_key_hash);
    
    return new_tx;
}

void print_input(Input *input){
    printf("\nInput Data:\n");
    printf("sig: %s\n", input->signature);
    printf("prev_tx_id: %s\n", input->prev_tx_id);
    printf("prev_output_num: %i\n", input->prev_utxo_output);
    printf("Size of Input(Bytes): %li\n", sizeof(Input));
}

void print_output(Output *output){
    printf("\nOutput Data:\n");
    printf("amt: %li\n", output->amt);
    printf("private Key: %s\n", output->public_key_hash);
    printf("Size Output(Bytes): %li\n", sizeof(Output));

}

void print_tx(Transaction *tx){
    printf("\nTransaction Data:\n");
    printf("num_inputs: %i\n", tx->num_inputs);
    printf("num_outputs: %i\n", tx->num_outputs);

    for(int i=0;i<tx->num_inputs;i++){
        print_input(&(tx->inputs[i]));
    }

    for(int i=0;i<tx->num_outputs;i++){
        print_output(&(tx->outputs[i]));
    }
   
    printf("Tx(Bytes): %li\n", sizeof(*tx)); // Check i
}

int main() {

    Output *an_Output = malloc(sizeof(Output));
    an_Output->amt = 5;
    strcpy(an_Output->public_key_hash, "a_val");

    Input *an_Input = malloc(sizeof(Input));
    an_Input->prev_utxo_output = 2;

    print_input(an_Input);
    print_output(an_Output);

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

    print_tx(a_Tx);

    // Other Access ways
    // printf("prev_tx_id[0]: %s\n", a_Tx->inputs->prev_tx_id);
    // printf("prev_tx_id[1]: %s\n", a_Tx->inputs[1].prev_tx_id);

    // printf("Output Amount: %li\n", a_Tx->outputs[0].amt);
    // printf("Output PubKey: %s\n", a_Tx->outputs[0].public_key_hash);
    // //printf("prev_tx_id: %s\n", ((a_Tx->inputs)+1)->prev_tx_id);

    //Serialization Testing
    char* char_tx = ser_Tx(a_Tx);

    Transaction * other_tx = deser_tx(char_tx);

    print_tx(other_tx);
    return 0;
}