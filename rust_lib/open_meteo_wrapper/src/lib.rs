use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::ptr;
use reqwest::blocking::Client;
use reqwest::Error;

/// get weather by lat/long coordinates
#[no_mangle]
pub unsafe extern "C" fn get_weather(lat: f64, lon: f64) -> *mut c_char {
    let url = format!(
        "https://api.open-meteo.com/v1/forecast?latitude={}&longitude={}&current_weather=true",
        lat, lon
    );

    let client = Client::new();
    let response = match client.get(&url).send() {
        Ok(resp) => resp,
        Err(_) => return ptr::null_mut(),
    };

    let body = match response.text() {
        Ok(text) => text,
        Err(_) => return ptr::null_mut(),
    };

    match CString::new(body) {
        Ok(c_string) => c_string.into_raw(),
        Err(_) => ptr::null_mut(),
    }
}

/// Get the weather data by city name
#[no_mangle]
pub unsafe extern "C" fn get_weather_by_city(city: *const c_char) -> *mut c_char {
    if city.is_null() {
        return ptr::null_mut();
    }

    let city_str = unsafe {
        CStr::from_ptr(city)
            .to_str()
            .unwrap_or_default()
    };

    let url = format!(
        "https://geocoding-api.open-meteo.com/v1/search?name={}&count=1",
        city_str
    );

    let client = Client::new();
    let response = match client.get(&url).send() {
        Ok(resp) => resp,
        Err(_) => return ptr::null_mut(),
    };

    let body = match response.text() {
        Ok(text) => text,
        Err(_) => return ptr::null_mut(),
    };

    // Get the lat and long values
    let json: serde_json::Value = match serde_json::from_str(&body) {
        Ok(val) => val,
        Err(_) => return ptr::null_mut(),
    };

    let lat = json["results"][0]["latitude"].as_f64().unwrap_or(0.0);
    let lon = json["results"][0]["longitude"].as_f64().unwrap_or(0.0);

    if lat == 0.0 && lon == 0.0 {
        return ptr::null_mut();
    }

    get_weather(lat, lon)
}

/// Free the allocated memory by the api
#[no_mangle]
pub unsafe extern "C" fn free_string(ptr: *mut c_char) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        drop(CString::from_raw(ptr));
    }
}
