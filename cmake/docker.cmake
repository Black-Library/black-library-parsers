macro(add_docker_targets)
    add_custom_target(bl-parsers-builder
        COMMAND docker build --force-rm --tag bl/parsers/builder --file tools/docker/bl_builder.docker .
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building bl-parsers-builder."
        VERBATIM)
endmacro()
