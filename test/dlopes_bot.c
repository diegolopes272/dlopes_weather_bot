
/*
 * Project: telebot weather api
 * Author: Diego Lopes
 * Date: Feb-2025
 * 
 * Brief: A simple C implementation of telegram bot interacting with open-meteo-api rust
 * 
 * dlopes_weather_bot
 * token: 7881074499:AAGjN5d-uawtOOH2gylNpLvBgCTcQpHFNgE
 */

/*****************************************************************************************/
// Includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <telebot.h>
#include "cJSON.h"

/*****************************************************************************************/
// Imported functions from Rust lib wrapper from open-meteo-api
extern char* get_weather(double lat, double lon);
extern char* get_weather_by_city(const char* city);
extern void free_string(char* ptr);


/*****************************************************************************************/
// Defines

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

#define BASE_RESPONSE "The current temperature for this location is: "

#define ERROR_RESPONSE "Invalid format received \n " \
                       "Hi, I'm dlopes_wheater_bot - You can consult the temperature of some location with me \n " \
                       "Please use one of the following formats to consulting:\n" \
                       "City name. For example: Bragança Paulista\n" \
                       "Or latitude and longitude coordinates. For example: -23.0, -46.5\n"

// generic errors type
enum {
    e_success = 0,
    e_error = 1         // basic generic error
};

/*****************************************************************************************/
// Prototypes
void testRoutines(void);

/*****************************************************************************************/
// Functions

/**
 * \brief  Check if the input is a GPS parseable coordinates
 * \param  input    Input value to try to parse
 * \param  lat      Return param: parsed latitude
 * \param  lon      Return param: parsed longitude
 * \return          e_success or e_error
 */
int parse_gps_coordinates(const char *input, double *lat, double *lon) {

    // try to parse and check the allowed interval for values 
    if (sscanf(input, " %lf , %lf ", lat, lon) == 2) {
        if (*lat >= -90.0 && *lat <= 90.0 && *lon >= -180.0 && *lon <= 180.0) {
            return e_success;
        }
    }
    return e_error;
}


/**
 * \brief  Get weather json info depending on received message 
 * \param  receive_msg      Input message - expected city name or gps coordinates
 * \param  response_msg     Output: The json weather data obtained
 * \return          e_success or e_error
 */
int getWeather(char *receive_msg, char *response_msg) {

    // Check if is lat/long or city message
    bool gps_coordinate = false;
    double lat, lon;
    if (parse_gps_coordinates(receive_msg, &lat, &lon) == e_success) {
        gps_coordinate = true;
    }

    // Try to get the weather depending of the type of the message
    char* weather_info = NULL;
    if (gps_coordinate) {
        
        // use coordinates to get the weather data
        weather_info = get_weather(lat, lon);
        if (weather_info == NULL) {
            printf("[getWeather] Error trying to get weather data by coordinate\n");
            return e_error;
        }
    
    }
    else {
        //if it is not coordinates, try to get by city name
        weather_info = get_weather_by_city(receive_msg);
        if (weather_info == NULL) {
            printf("[getWeather] Error trying to get weather data by city name\n");
            return e_error;
        }
    }

    memcpy(response_msg, weather_info, 1024);
    free_string(weather_info);
    return e_success;

}

/**
 * \brief  Get temperature from weather json info 
 * \param  receive_weather    Input json format data with weather information
 * \param  temperature_info   Output: the extrated temperature
 * \return          e_success or e_error
 */
int getTemperatureInfo(char *receive_weather, char *temperature_info) {

    printf("\n[getTemperatureInfo] receive_weather:%s\n\n", receive_weather);

    cJSON *json = cJSON_Parse(receive_weather);
    if (!json) {
        printf("[getTemperatureInfo] Error parsing Json from receive_msg\n");
        return e_error;
    }

    cJSON *current_weather = cJSON_GetObjectItem(json, "current_weather");
    if (!current_weather) {
        printf("[getTemperatureInfo] Error 'current_weather' field not found\n");
        cJSON_Delete(json);
        return e_error;
    }

    cJSON *temperature = cJSON_GetObjectItem(current_weather, "temperature");
    if (!cJSON_IsNumber(temperature)) {
        printf("[getTemperatureInfo] Error 'temperature' object not found\n");
        cJSON_Delete(json);
        return e_error;
    }

    char temperature_str[16];
    snprintf(temperature_str, sizeof(temperature_str), "%.1f °C", temperature->valuedouble);

    printf("[getTemperatureInfo] Temperature OK: %s\n", temperature_str);
    memcpy(temperature_info, temperature_str, 16);

    cJSON_Delete(json);
    return e_success;

}

/**
 * \brief  Initialize and get basic info from telebot handle
 * \param  handle    pointer to a telebot_handler_t
 * \return           e_success or e_error
 */
int telebotInit(telebot_handler_t *handle) {

    // open the token file genereted by botfather
    FILE *fp = fopen(".token", "r");
    if (fp == NULL) {
        printf("[telebotInit] Failed to open .token file\n");
        return e_error;
    }
    char token[1024];
    if (fscanf(fp, "%s", token) == 0) {
        printf("[telebotInit] Failed to read token\n");
        fclose(fp);
        return e_error;
    }
    printf("[telebotInit] Token: %s\n", token);
    fclose(fp);


    // create the handle
    if (telebot_create(handle, token) != TELEBOT_ERROR_NONE) {
        printf("[telebotInit] Telebot create failed\n");
        return e_error;
    }

    // get info from bot
    telebot_user_t me;
    if (telebot_get_me(*handle, &me) != TELEBOT_ERROR_NONE) {
        printf("[telebotInit] Failed to get bot information\n");
        telebot_destroy(*handle);
        return e_error;
    }
    printf("[telebotInit] Telebot info:\n");
    printf("[telebotInit] ID: %d\n", me.id);
    printf("[telebotInit] First Name: %s\n", me.first_name);
    printf("[telebotInit] User Name: %s\n", me.username);
    telebot_put_me(&me);

    return e_success;

}


/**
 * \brief   main program function
 * \return  e_success or e_error
 */
int main(int argc, char *argv[])
{
    printf("[main] Starting dlopes_weather telegram bot..\n");


    #if RUN_TEST
    testRoutines();
    return e_success;
    #endif

    telebot_handler_t handle;
    if (telebotInit(&handle) != e_success) {
        printf("[main] Failed to telebotInit\n");
        return e_error;
    }

    int index, count, offset = -1;
    telebot_error_e ret;
    telebot_message_t message;
    telebot_update_type_e update_types[] = {TELEBOT_UPDATE_TYPE_MESSAGE};

    while (1)
    {
        telebot_update_t *updates;
        ret = telebot_get_updates(handle, offset, 20, 0, update_types, 0, &updates, &count);
        if (ret != TELEBOT_ERROR_NONE)
            continue;
        printf("[main] Number of updates: %d\n", count);

        // for each update
        for (index = 0; index < count; index++) {
            message = updates[index].message;
            if (message.text) {
                printf("[main] Message from:%s %s: %s \n", message.from->first_name, message.from->last_name, message.text);
                
                char ack_message[4096];
                char weather_info[1024];
                int rv;

                // check the received message (coordinate or city and get the weather info)
                rv = getWeather(message.text, weather_info);
                if(rv == e_success) {

                    // parse the temperature info from weather_info and prepare the ack_message
                    char temperature_str[16];
                    rv = getTemperatureInfo(weather_info, temperature_str);
                    if (rv == e_success) {
                        snprintf(ack_message, SIZE_OF_ARRAY(ack_message), "<i>%s%s</i>",
                                    BASE_RESPONSE, temperature_str);
                    }
                }

                // error on trying to get temperature
                // set the instruction message to the user
                if (rv == e_error) {
                    snprintf(ack_message, SIZE_OF_ARRAY(ack_message), "<i>%s</i>",
                                ERROR_RESPONSE);
                }

                // reply the message
                ret = telebot_send_message(handle, message.chat->id, ack_message, "HTML", false, false, updates[index].message.message_id, "");
                if (ret != TELEBOT_ERROR_NONE) {
                    printf("Failed to send message: %d \n", ret);
                }

            } //if message.text

            // get the next update
            offset = updates[index].update_id + 1;
        
        } //for

        // free the update message
        telebot_put_updates(updates, count);

        sleep(1);
    
    } // while 1

    telebot_destroy(handle);

    return 0;
}




/*****************************************************************************************/
// Simple Tests - enable RUN_TEST on test/CMakeLists.txt to build and run it

#if RUN_TEST
/**
 * \brief  Simple test routines to check the created functions
 */
void testRoutines(void) {

    printf("[testRoutines] Initializing tests...\n");

    // Test parse_gps_coordinates
    double lat, lon;
    if (parse_gps_coordinates("-23.0, -46.5", &lat, &lon) == e_success) {
        printf("[testRoutines] parse_gps_coordinates OK: lat=%.1f, lon=%.1f\n", lat, lon);
    } else {
        printf("[testRoutines] parse_gps_coordinates FAIL\n");
    }

    // Test getWeather with city
    char response_msg[1024] = { 0 };
    if (getWeather("Braganca Paulista", response_msg) == e_success) {
        printf("[testRoutines] getWeather city OK: %s\n", response_msg);
    } else {
        printf("[testRoutines] getWeather city FAIL\n");
    }

    // Test getWeather with coordinates
    memset(response_msg, 0, sizeof(response_msg));
    if (getWeather("-23.5, -46.1", response_msg) == e_success) {
        printf("[testRoutines] getWeather coord OK: %s\n", response_msg);
    } else {
        printf("[testRoutines] getWeather coord FAIL\n");
    }

    // Test getTemperatureInfo
    char temperature_info[16] = { 0 };
    const char *mock_json = "{\"current_weather\": {\"temperature\": 25.5}}";
    if (getTemperatureInfo((char *)mock_json, temperature_info) == e_success) {
        printf("[testRoutines] getTemperatureInfo OK: %s\n", temperature_info);
    } else {
        printf("[testRoutines] getTemperatureInfo FAIL\n");
    }

    printf("[testRoutines] End of the tests.\n");
}
#endif

