#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <glm/glm.hpp>
#include <iostream>


/*
  Carrega um modelo 3D a partir de um ficheiro OBJ.
  Lê vértices, coordenadas de textura e normais do ficheiro e armazena-os nos vetores fornecidos.
  Suporta apenas faces triangulares e quadrangulares (que são convertidas em triângulos).
  Retorna true se o carregamento for bem-sucedido, false caso contrário.
 */
bool loadOBJ(
    const char * path, 
    std::vector<glm::vec3> & out_vertices, 
    std::vector<glm::vec2> & out_uvs, 
    std::vector<glm::vec3> & out_normals
){
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices; 
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossível abrir o ficheiro! Estás no diretório correto?\n");
        return false;
    }

    while( 1 ){

        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; 
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; 
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            char line[1024];
            fgets(line, 1024, file);
            
            unsigned int vIndex[4], uvIndex[4], nIndex[4];
            int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                &vIndex[0], &uvIndex[0], &nIndex[0],
                &vIndex[1], &uvIndex[1], &nIndex[1],
                &vIndex[2], &uvIndex[2], &nIndex[2],
                &vIndex[3], &uvIndex[3], &nIndex[3]);

            if (matches == 9 || matches == 12) {
                vertexIndices.push_back(vIndex[0]);
                vertexIndices.push_back(vIndex[1]);
                vertexIndices.push_back(vIndex[2]);
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
                normalIndices.push_back(nIndex[0]);
                normalIndices.push_back(nIndex[1]);
                normalIndices.push_back(nIndex[2]);

                if (matches == 12) {
                    vertexIndices.push_back(vIndex[0]);
                    vertexIndices.push_back(vIndex[2]);
                    vertexIndices.push_back(vIndex[3]);
                    uvIndices    .push_back(uvIndex[0]);
                    uvIndices    .push_back(uvIndex[2]);
                    uvIndices    .push_back(uvIndex[3]);
                    normalIndices.push_back(nIndex[0]);
                    normalIndices.push_back(nIndex[2]);
                    normalIndices.push_back(nIndex[3]);
                }
            } else {
                 matches = sscanf(line, "%d//%d %d//%d %d//%d %d//%d", 
                    &vIndex[0], &nIndex[0],
                    &vIndex[1], &nIndex[1],
                    &vIndex[2], &nIndex[2],
                    &vIndex[3], &nIndex[3]);
                 
                 if (matches == 6 || matches == 8) {
                    vertexIndices.push_back(vIndex[0]);
                    vertexIndices.push_back(vIndex[1]);
                    vertexIndices.push_back(vIndex[2]);
                    normalIndices.push_back(nIndex[0]);
                    normalIndices.push_back(nIndex[1]);
                    normalIndices.push_back(nIndex[2]);
                    uvIndices.push_back(0); uvIndices.push_back(0); uvIndices.push_back(0);

                    if (matches == 8) {
                        vertexIndices.push_back(vIndex[0]);
                        vertexIndices.push_back(vIndex[2]);
                        vertexIndices.push_back(vIndex[3]);
                        normalIndices.push_back(nIndex[0]);
                        normalIndices.push_back(nIndex[2]);
                        normalIndices.push_back(nIndex[3]);
                        uvIndices.push_back(0); uvIndices.push_back(0); uvIndices.push_back(0);
                    }
                 } else {
                     printf("Ficheiro não pode ser lido pelo nosso parser simples :-( Matches: %d\n", matches);
                 }
            }
        }
    }

    for( unsigned int i=0; i<vertexIndices.size(); i++ ){
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        out_vertices.push_back(vertex);

        if (!uvIndices.empty() && uvIndices[i] != 0 && uvIndices[i] <= temp_uvs.size()) {
            unsigned int uvIndex = uvIndices[i];
            glm::vec2 uv = temp_uvs[ uvIndex-1 ];
            out_uvs.push_back(uv);
        } else {
            out_uvs.push_back(glm::vec2(0.0f));
        }

        if (!normalIndices.empty() && normalIndices[i] != 0 && normalIndices[i] <= temp_normals.size()) {
            unsigned int normalIndex = normalIndices[i];
            glm::vec3 normal = temp_normals[ normalIndex-1 ];
            out_normals.push_back(normal);
        } else {
            out_normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }
    fclose(file);
    return true;
}

#endif
