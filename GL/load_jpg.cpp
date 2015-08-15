
GLuint loadjpgGL (const char* file);
GLuint loadjpgGL (const char* file)
    {
    unsigned char* buffer;
    JPEG_CORE_PROPERTIES image = {0};

    if (ijlInit (&image) != IJL_OK)
        {
        printf ("jpg %s is not loaded::can not init ijl \n", file);
        return 0;
        }

    image.JPGFile = file;

    if (ijlRead (&image, IJL_JFILE_READPARAMS) != IJL_OK)
        {
        printf ("jpg %s is not loaded::corrupted jpg \n", file);
        return 0;
        }

    buffer = new unsigned char[image.JPGWidth * image.JPGHeight * 3];
    image.DIBBytes = buffer;
    image.DIBColor = IJL_RGB;
    image.DIBHeight = image.JPGHeight;
    image.DIBWidth = image.JPGWidth;
    image.DIBChannels = 3;


    if (ijlRead (&image, IJL_JFILE_READWHOLEIMAGE) == IJL_OK)
        {
        GLuint handle;
        glGenTextures (1, &handle);
        glBindTexture (GL_TEXTURE_2D, handle);

        glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D (
            GL_TEXTURE_2D,
            0,
            GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
            image.JPGHeight,
            image.JPGWidth,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            buffer);


        delete buffer;
        ijlFree (&image);

        printf ("jpg %s is loaded:: X=%i Y=%i, OpenGL desc %i\n",
                file, image.JPGHeight, image.JPGWidth, handle);

        return handle;
        }

    delete buffer;
    ijlFree (&image);

    printf ("jpg %s is not loaded::corrupted jpg \n", file);


    return 0;
    }


void   loadjpgSW (const char* file, unsigned char*& buffer, int& X, int& Y);
void   loadjpgSW (const char* file, unsigned char*& buffer, int& X, int& Y)
    {
    JPEG_CORE_PROPERTIES image = {0};

    if (ijlInit (&image) != IJL_OK)
        {
        printf ("jpg %s is not loaded::can not init ijl \n", file);
        return;
        }

    image.JPGFile = file;

    if (ijlRead (&image, IJL_JFILE_READPARAMS) != IJL_OK)
        {
        printf ("jpg %s is not loaded::corrupted jpg \n", file);
        return;
        }


    buffer = new unsigned char[image.JPGWidth * image.JPGHeight * 3];
    image.DIBBytes = buffer;
    image.DIBColor = IJL_RGB;
    image.DIBHeight = image.JPGHeight;
    image.DIBWidth = image.JPGWidth;
    image.DIBChannels = 3;


    if (ijlRead (&image, IJL_JFILE_READWHOLEIMAGE) == IJL_OK)
        {

        X = image.JPGHeight;
        Y = image.JPGWidth;

        printf ("jpg %s is loaded:: X=%i Y=%i \n", file, X, Y);
        }
    else
        {

        printf ("jpg %s is not loaded \n", file);
        delete buffer;
        buffer = NULL;
        X = Y = 0;
        }

    }
