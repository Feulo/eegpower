#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265
#define TABLE_SIZE 2048

typedef struct t_data{
    float * sine_table;
    float index;
}sine_data;


float *  create_sine_table(float * table){
    table = malloc(TABLE_SIZE * sizeof(float));
    int i;
    for(i=0; i<TABLE_SIZE; i++){
        table[i] = sin(2.0*PI*i/TABLE_SIZE);
    }
	return table;
}

/*
Standar function to get interpolated values in a sine table
*/
float get_interpolated_freq(sine_data * data, float frequency, float sr){

	int my_floor = floor(data->index);
	float y = data->index - my_floor;

	// Definicao de indexs circulares
	int index1= (my_floor-1 >= 0)?my_floor - 1:TABLE_SIZE + (my_floor-1);
	int index2= my_floor;
	int index3= (my_floor+1 < TABLE_SIZE)?my_floor + 1:my_floor + 1 - TABLE_SIZE;
	int index4= (my_floor+2 < TABLE_SIZE)?my_floor + 2:my_floor + 2 - TABLE_SIZE;
//	float v_interpolado = table[my_floor]; // SEM INTERPOLACAO
//	float v_interpolado = ((1.0 - (y)) * table[index2] + (y) * table[index3]); // INTERPOLACAO LINEAR

	float v_interpolado =    - ((y    ) * (y - 1) * (y - 2) * data->sine_table[index1])/6    //INTERPOLACAO CUBICA
				   + ((y + 1) * (y - 1) * (y - 2) * data->sine_table[index2])/2
				   - ((y + 1) * (y    ) * (y - 2) * data->sine_table[index3])/2
				   + ((y + 1) * (y    ) * (y - 1) * data->sine_table[index4])/6;

	// proximo index a ser lido para esta frequencia
	data->index += TABLE_SIZE * frequency / sr; // Tamanho da tabela * frequencia / Sample rate
	if (data->index >= TABLE_SIZE) //Truncamento pelo tamanho da tabela seno
		data->index -= TABLE_SIZE;

	return v_interpolado;
}

