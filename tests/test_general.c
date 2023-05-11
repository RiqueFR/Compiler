void printf(){
	/* so para ele parar de quebrar*/
}

void print( char* string) {
	printf("%s", string);
}

int calcula(int x, int y){
	return x + y;
}

// main funtion
int main (  )      		{
	int x = calcula ( 2, 6 );
	int y = calcula ( 7 , 10);
	
	/** variable to store the formula
	 *  that is the result value
	 *  it is a float value
	 *
	 */
	float res = x /4.56 * y;
	
	char* sTring_OuT2 ="That is not a if, it is a char*";
	
	int int_array[3] = {0, 2, 4};

	int aux = 0;
	while ( aux < 3) {
		res = res + int_array[aux];
		aux = aux + 1;
	}

	if((x == y && x > 3 && y < 10) || !(res > 0)) {
		return 1;
	} else {
		if (res > 0) {
			return 2;
		} else {
			return 0;
		}
	} 
	return 0;


}
