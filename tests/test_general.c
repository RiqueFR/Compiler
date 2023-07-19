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
	printf(x);
	printf(y);
	printf(res);
	
	char* sTring_OuT2 ="That is not a if, it is a char*\n";
	printf(sTring_OuT2);
	
	int int_array[3];
	int_array[0] = 0;
	int_array[1] = 2;
	int_array[2] = 4;

	int aux = 0;
	while ( aux < 3) {
		res = res + int_array[aux];
		printf(res);
		aux = aux + 1;
	}

	if(res == y/*(x == y && x > 3 && y < 10) || !(res > 0)*/) {
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
