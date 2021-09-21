#include <k4a/k4a.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[]){

    uint32_t dev_count = k4a_device_get_installed_count();
    if(dev_count == 0){
        printf("No k4a devices attached!\n");
        return 1;
    }

    // Open the device
    k4a_device_t device = NULL;
    k4a_device_open(K4A_DEVICE_DEFAULT, &device);

    // Get the sisze of the serial number
    size_t serial_size = 0;
    k4a_device_get_serialnum(device, NULL,&serial_size);

    //Allocate memory for the serial, then acquire it
    char* serial = (char*) (malloc(serial_size));
    k4a_device_get_serialnum(device,serial,&serial_size);
    printf("Opened device: %s\n", serial);
    free(serial);

    // Configure a stream of ***4096x3072 BGRA*** color data at 15 frames per second

    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_15;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG; // BGRA32?
    config.color_resolution = K4A_COLOR_RESOLUTION_2160P; // 
    config.depth_mode = K4A_DEPTH_MODE_WFOV_UNBINNED; // Wide field of vision unbinned

    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        goto Exit;
    }
    
    // ...Camera capture and application specific code would go here...

    // Capture a depth frame
    switch (k4a_device_get_capture(device, &capture, TIMEOUT_IN_MS))
    {
    case K4A_WAIT_RESULT_SUCCEEDED:
        break;
    case K4A_WAIT_RESULT_TIMEOUT:
        printf("Timed out waiting for a capture\n");
        continue;
        break;
    case K4A_WAIT_RESULT_FAILED:
        printf("Failed to read a capture\n");
        goto Exit;
    }

    // Access the depth16 image
    k4a_image_t depth_image = k4a_capture_get_depth_image(capture);
    if (depth_image != NULL)
    {
        printf(" | Depth16 res:%4dx%4d stride:%5d\n",
                k4a_image_get_height_pixels(depth_image),
                k4a_image_get_width_pixels(depth_image),
                k4a_image_get_stride_bytes(depth_image));

        // Release the image
        k4a_image_release(depth_image);
    }


    // Access the color image
    k4a_image_t color_image = k4a_capture_get_depth_image(capture);
    if (color_image != NULL)
    {
        printf(" | Color res:%4dx%4d stride:%5d\n",
                k4a_image_get_height_pixels(color_image),
                k4a_image_get_width_pixels(color_image),
                k4a_image_get_stride_bytes(color_image));

        // Release the image
        k4a_image_release(color_image);
    }
    // Release the capture
    k4a_capture_release(capture);
    // Shut down the camera when finished with application logic
    k4a_device_stop_cameras(device);

    // Close the opened device
    k4a_device_close(device);

    return 0;
}

// GITHUB TOKEN
// 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
