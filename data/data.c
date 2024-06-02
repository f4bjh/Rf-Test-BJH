get_measurement()
{

    json_data_t json_data; 

    json_data.value = malloc( 32*sizeof(char)); 
    memset(json_data.value, 0, 32*sizeof(char));


    //here we should add a switch case, base on a queue intertask data
    //in wich, we choose wich data should be updated
    get_chip_info_model(json_data.value, &json_data.length);

    if (json_data.length !=0) {

        json_data.tag = CHIP_INFO_MODEL_DATA_TAG;//should be the same value as in the switch case

    	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "tag", json_data.tag);
    	cJSON_AddNumberToObject(root, "length", json_data.length);
    	cJSON_AddStringToObject(root, "value", json_data.value);

    	char *json_string = cJSON_Print(root);
    	free(json_data.value);
    	cJSON_Delete(root);
	
	envoyer dans la queue 
     }

}




void data_init(void)
{

create task on cpu1 get_measurment
	create queue


}
