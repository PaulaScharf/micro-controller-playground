set(src_dirs    
                ai
                )

set(include_dirs    
                    ai
                    )

set(requires    esp32-camera
                esp-dl
                fb_gfx)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires} EMBED_FILES ${embed_files})

component_compile_options(-ffast-math -O3)
