﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
    /* A 3-part data structure */
    class Vector3
    {
        public Vector3() { }
        public Vector3(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }
        public float x = 0.0f;
        public float y = 0.0f;
        public float z = 0.0f;
    }

    /* A 4-part data structure for colours */
    class Colour
    {
        public Colour() { }
        public Colour(float _r, float _g, float _b, float _a)
        {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        }
        public float r = 0;
        public float g = 0;
        public float b = 0;
        public float a = 0;
    }

    /* A 2-part data structure */
    class Vector2
    {
        public Vector2() { }
        public Vector2(float _x, float _y)
        {
            x = _x;
            y = _y;
        }
        public float x = 0.0f;
        public float y = 0.0f;
    }

    /* A face, made up of vertices, each with a material name */
    class Face
    {
        public List<Vertex> verts = new List<Vertex>();
        public string materialName = "";
    }

    /* A vertex */
    class Vertex
    {
        public int v = 0;
        public int c = 0;
        public int n = 0;
        public bool set = false;
    }

    /* A material */
    class Material
    {
        public string materialName = "";
        public string texturePath = "";
        public Colour colourTint = new Colour();
    }
    
    /* OBJ reader state */
    enum VertReaderType
    {
        VERTEX,
        COORDINATE,
        NORMAL,
    };

    /* A part of a model with its associated material data */
    class ModelPart
    {
        //Geometry
        public List<Face> modelFaces = new List<Face>();

        //Material
    }

    /* A model, made up of parts with materials */
    class Model
    {
        public List<ModelPart> modelParts = new List<ModelPart>();

        public LoadOBJ(string path)
        {
            //Parse the OBJ to vertices/texcoords/normals
            List<string> objFile = File.ReadAllLines(path).ToList();
            List<Vector3> verts = new List<Vector3>();
            List<Vector2> coords = new List<Vector2>();
            List<Vector3> normals = new List<Vector3>();
            List<Face> faces = new List<Face>();
            string materialLibrary = "";
            string thisMaterial = "";
            foreach (string str in objFile) 
            {
                if (str.Length > 0)
                {
                    if (str.Length > 7 && str.Substring(0, 7) == "mtllib ")
                    {
                        materialLibrary = str.Substring(7);
                    }
                    else if (str.Length > 2 && str.Substring(0, 2) == "v ")
                    {
                        string _str = str.Substring(2);
                        string thisPos = "";
                        Vector3 thisVertPos = new Vector3();
                        int vertPosIndex = 0;
                        for (int i = 0; i < _str.Length + 1; i++)
                        {
                            if (_str[i] == ' ' || i == _str.Length)
                            {
                                if (thisPos == "") continue;
                                if (vertPosIndex == 0) thisVertPos.x = Convert.ToSingle(thisPos);
                                if (vertPosIndex == 1) thisVertPos.y = Convert.ToSingle(thisPos);
                                if (vertPosIndex == 2) thisVertPos.z = Convert.ToSingle(thisPos);
                                vertPosIndex++;
                                thisPos = "";
                                continue;
                            }
                            thisPos += _str[i];
                        }
                        verts.Add(thisVertPos);
                    }
                    else if (str.Length > 3 && str.Substring(0, 3) == "vt ")
                    {
                        string _str = str.Substring(3);
                        string thisPos = "";
                        Vector2 thisTexCoord = new Vector2();
                        int texCoordIndex = 0;
                        for (int i = 0; i < _str.Length + 1; i++)
                        {
                            if (_str[i] == ' ' || i == _str.Length)
                            {
                                if (thisPos == "") continue;
                                if (texCoordIndex == 0) thisTexCoord.x = Convert.ToSingle(thisPos);
                                if (texCoordIndex == 1) thisTexCoord.y = Convert.ToSingle(thisPos) * -1;
                                texCoordIndex++;
                                thisPos = "";
                                continue;
                            }
                            thisPos += _str[i];
                        }
                        coords.Add(thisTexCoord);
                    }
                    else if (str.Length > 3 && str.Substring(0, 3) == "vn ")
                    {
                        string _str = str.Substring(3);
                        string thisPos = "";
                        Vector3 thisNormal = new Vector3();
                        int normalIndex = 0;
                        for (int i = 0; i < _str.Length + 1; i++)
                        {
                            if (_str[i] == ' ' || i == _str.Length)
                            {
                                if (thisPos == "") continue;
                                if (normalIndex == 0) thisNormal.x = Convert.ToSingle(thisPos);
                                if (normalIndex == 1) thisNormal.y = Convert.ToSingle(thisPos);
                                if (normalIndex == 2) thisNormal.z = Convert.ToSingle(thisPos);
                                normalIndex++;
                                thisPos = "";
                                continue;
                            }
                            thisPos += _str[i];
                        }
                        normals.Add(thisNormal);
                    }
                    else if (str.Length > 7 && str.Substring(0, 7) == "usemtl ")
                    {
                        thisMaterial = str.Substring(7);
                    }
                    else if (str.Length > 2 && str.Substring(0, 2) == "f ")
                    {
                        string _str = str.Substring(2);
                        Face thisFace = new Face();
                        Vertex thisVert = new Vertex();
                        VertReaderType next = VertReaderType.VERTEX;
                        string currentNumber = "";
                        for (int i = 0; i < _str.Length + 1; i++)
                        {
                            char thisChar = _str[i];
                            if (thisChar == '/' || thisChar == ' ' || i == _str.Length)
                            {
                                if (currentNumber == "")
                                {
                                    //if (verts.Count == 0) Debug::Log("This model has no vertices!");
                                    //if (coords.Count == 0) Debug::Log("This model has no UVs!");
                                    //if (normals.Count == 0) Debug::Log("This model has no normals!");
                                    continue;
                                }
                                switch (next)
                                {
                                    case VertReaderType.VERTEX:
                                        thisVert.v = Convert.ToInt32(currentNumber);
                                        next = VertReaderType.COORDINATE;
                                        break;
                                    case VertReaderType.COORDINATE:
                                        thisVert.c = Convert.ToInt32(currentNumber);
                                        next = VertReaderType.NORMAL;
                                        break;
                                    case VertReaderType.NORMAL:
                                        thisVert.n = Convert.ToInt32(currentNumber);
                                        next = VertReaderType.VERTEX;
                                        break;
                                }
                                thisVert.set = true;
                                currentNumber = "";

                                if (thisChar == '/') continue;
                                if (!thisVert.set) continue;
                                thisFace.verts.Add(thisVert);
                                thisVert = new Vertex();
                                continue;
                            }
                            currentNumber += thisChar;
                        }
                        //if (thisFace.verts.Count != 3) Debug::Log("This model is not triangulated!");
                        thisFace.materialName = thisMaterial;
                        faces.Add(thisFace);
                    }
                }
            }

            //Open and parse MTL if it exists
            List<Material> materials = new List<Material>();
            if (materialLibrary != "")
            {
                //Get model path parts
                List<string> modelPath = new List<string>();
                string currentPath = "";
                for (int i = 0; i < path.Length; i++)
                {
                    if (path[i] == '/' || path[i] == '\\')
                    {
                        modelPath.Add(currentPath);
                        currentPath = "";
                        continue;
                    }
                    currentPath += path[i];
                }
                modelPath.Add(currentPath);

                //Get material path parts
                List<string> mtlPath = new List<string>();
                currentPath = "";
                for (int i = 0; i < materialLibrary.Length; i++)
                {
                    if (materialLibrary[i] == '/' || materialLibrary[i] == '\\')
                    {
                        mtlPath.Add(currentPath);
                        currentPath = "";
                        continue;
                    }
                    currentPath += materialLibrary[i];
                }
                mtlPath.Add(currentPath);

                //Reconstruct a good path to MTL using model path
                string pathToMtl = "";
                if (mtlPath.Count == 1)
                {
                    if (modelPath.Count == 1)
                    {
                        pathToMtl = mtlPath[0];
                    }
                    else
                    {
                        for (int i = 0; i < modelPath.Count - 1; i++)
                        {
                            pathToMtl += modelPath[i] + "/";
                        }
                        pathToMtl += mtlPath[0];
                    }
                }
                else
                {
                    pathToMtl = materialLibrary;
                }

                //Open MTL
                List<string> mtlFile = File.ReadAllLines(pathToMtl).ToList();

                //Parse MTL into materials
                Material currentMaterial = new Material();
                foreach (string str in mtlFile)
                {
                    if (str.Length > 0)
                    {
                        if (str.Length > 7 && str.Substring(0, 7) == "newmtl ")
                        {
                            if (currentMaterial.materialName != "")
                            {
                                materials.Add(currentMaterial);
                                currentMaterial = new Material();
                            }
                            currentMaterial.materialName = str.Substring(7);
                        }
                        else if (str.Length > 3 && str.Substring(0, 3) == "Kd ")
                        {
                            string _str = str.Substring(3);
                            string thisColour = "";
                            int thisColourIndex = 0;
                            for (int i = 0; i < _str.Length + 1; i++)
                            {
                                if (_str[i] == ' ' || i == _str.Length)
                                {
                                    if (thisColour == "") continue;
                                    if (thisColourIndex == 0) currentMaterial.colourTint.r = Convert.ToSingle(thisColour);
                                    if (thisColourIndex == 1) currentMaterial.colourTint.g = Convert.ToSingle(thisColour);
                                    if (thisColourIndex == 2) currentMaterial.colourTint.b = Convert.ToSingle(thisColour);
                                    thisColourIndex++;
                                    thisColour = "";
                                    continue;
                                }
                                thisColour += _str[i];
                            }
                        }
                        else if (str.Length > 2 && str.Substring(0, 2) == "d ")
                        {
                            currentMaterial.colourTint.a = Convert.ToSingle(str.Substring(2));
                        }
                        else if (str.Length > 7 && str.Substring(0, 7) == "map_Kd ")
                        {
                            currentMaterial.texturePath = str.Substring(7);
                            if (currentMaterial.texturePath[1] == ':')
                            {
                                //Debug::Log("Texture uses system path! " + currentMaterial.texturePath);
                            }
                            else
                            {
                                string texPrepend = "";
                                for (int i = pathToMtl.Length; i >= 0; i--)
                                {
                                    if (pathToMtl[i] == '/' || pathToMtl[i] == '\\')
                                    {
                                        texPrepend = pathToMtl.Substring(0, i);
                                        break;
                                    }
                                }
                                currentMaterial.texturePath = texPrepend + "/" + currentMaterial.texturePath;
                            }
                        }
                    }
                }
                if (currentMaterial.materialName != "")
                {
                    materials.Add(currentMaterial);
                }
            }

            //Create vertex and index arrays from the data
            ModelPart modelPart = new ModelPart();
            int totalIndex = 0;
            for (int i = 0; i < faces.size(); i++)
            {
                for (int x = 0; x < faces[i].verts.size(); x++)
                {
                    Vertex thisVertInfo = new Vertex();
                    thisVertInfo.Pos = verts[faces[i].verts[x].v - 1];
                    thisVertInfo.Tex = coords[faces[i].verts[x].c - 1];
                    thisVertInfo.Normal = normals[faces[i].verts[x].n - 1];

                    if (modelPart.thisMaterial.materialName != faces[i].materialName)
                    {
                        if (totalIndex != 0)
                        {
                            thisModel.modelParts.push_back(modelPart);
                            modelPart = LoadedModelPart();
                        }
                        for (int y = 0; y < materials.size(); y++)
                        {
                            if (materials[y].materialName == faces[i].materialName)
                            {
                                modelPart.thisMaterial = materials[y];
                                break;
                            }
                        }
                    }

                    modelPart.compVertices.push_back(thisVertInfo);
                    modelPart.compIndices.push_back((WORD)totalIndex);

                    totalIndex++;
                }
            }
            thisModel.modelParts.push_back(modelPart);
            return thisModel;
        }
    }
}