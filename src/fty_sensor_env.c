/*  =========================================================================
    fty_sensor_env - Runs fty-sensor-env-server class

    Copyright (C) 2014 - 2017 Eaton                                        
                                                                           
    This program is free software; you can redistribute it and/or modify   
    it under the terms of the GNU General Public License as published by   
    the Free Software Foundation; either version 2 of the License, or      
    (at your option) any later version.                                    
                                                                           
    This program is distributed in the hope that it will be useful,        
    but WITHOUT ANY WARRANTY; without even the implied warranty of         
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
    GNU General Public License for more details.                           
                                                                           
    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.            
    =========================================================================
*/

/*!
 * \file fty_sensor_env.cc
 * \author Michal Hrusecky <MichalHrusecky@Eaton.com>
 * \author Tomas Halman <TomasHalman@Eaton.com>
 * \author Jim Klimov <EvgenyKlimov@Eaton.com>
 * \author Jiri Kukacka <JiriKukacka@Eaton.com>
 * \brief Not yet documented file
 */
/*
@header
    fty_sensor_env - Runs fty-sensor-env-server class
@discuss
@end
*/

#include "fty_sensor_env_classes.h"

static const char *ACTOR_NAME = "fty-sensor-env";
static const char *ENDPOINT = "ipc://@/malamute";

int main (int argc, char *argv [])
{
    bool verbose = false;
    int argn;
    for (argn = 1; argn < argc; argn++) {
        const char *param = NULL;
        if (argn < argc - 1) param = argv [argn+1];

        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-h")) {
            puts ("fty-sensor-env [options] ...");
            puts ("  --verbose / -v         verbose test output");
            puts ("  --help / -h            this information");
            puts ("  --endpoint / -e        malamute endpoint [ipc://@/malamute]");
            return 0;
        }
        else if (streq (argv [argn], "--verbose") || streq (argv [argn], "-v")) {
            verbose = true;
        }
        else if (streq (argv [argn], "--endpoint") || streq (argv [argn], "-e")) {
            if (param) ENDPOINT = param;
            ++argn;
        }
        else {
            printf ("Unknown option: %s\n", argv [argn]);
            return 1;
        }
    }
    if (verbose)
        zsys_info ("fty_sensor_env - started");
    zactor_t *server = zactor_new (sensor_env_actor, NULL);
    assert (server);
    zstr_sendx (server, "BIND", ENDPOINT, ACTOR_NAME, NULL);
    zstr_sendx (server, "PRODUCER", FTY_PROTO_STREAM_METRICS_SENSOR, NULL);
    zstr_sendx (server, "CONSUMER", FTY_PROTO_STREAM_ASSETS, ".*", NULL);
    if (verbose)
        zstr_sendx (server, "VERBOSE", NULL);

    while (!zsys_interrupted) {
        zmsg_t *msg = zactor_recv (server);
        zmsg_destroy (&msg);
    }   
    zactor_destroy (&server);
    if (verbose)
        zsys_info ("fty_sensor_env - exited");

    return 0;
}