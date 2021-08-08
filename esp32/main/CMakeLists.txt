# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

idf_component_register(SRCS "greenhouse.c" "wifi.c" "tcp-client.c" INCLUDE_DIRS ".")
