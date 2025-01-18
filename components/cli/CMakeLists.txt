idf_component_register(SRCS "cli.c"
                    INCLUDE_DIRS "."
                    REQUIRES console esp_driver_uart
                    PRIV_REQUIRES 
                                  ""
                                  "cli"
                                  "wifi"
                                  )