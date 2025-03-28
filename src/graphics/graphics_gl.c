/*typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, bool32, const int*);*/
// void GWindowGLInit(GWindow *gw)
// {
// 	XSetWindowAttributes attr = {0,};

//     gw->alive = true;
//     gw->event = (XEvent){0};
//     gw->key = (KeySym){0};
//     gw->width = screenWidth;
//     gw->height = screenHeight;

//     gw->display = XOpenDisplay((char *)NULL);
//     gw->screen = DefaultScreen(gw->display);
//     gw->root = RootWindow(gw->display, gw->screen);

//     int visdata[] = {
//         GLX_RENDER_TYPE, GLX_RGBA_BIT,
//         GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
//         GLX_DOUBLEBUFFER, True,
//         GLX_RED_SIZE, 8,
//         GLX_GREEN_SIZE, 8,
//         GLX_BLUE_SIZE, 8,
//         GLX_ALPHA_SIZE, 8,
//         GLX_DEPTH_SIZE, 16,
//         None
//     };

//     gw->fbconfigs = glXChooseFBConfig(gw->display, gw->screen, visdata, &gw->numfbconfigs);
//     gw->fbconfig = 0;

//     for (uint32 i = 0; i < gw->numfbconfigs; i++)
//     {
//         gw->visual = (XVisualInfo *)glXGetVisualFromFBConfig(gw->display, gw->fbconfigs[i]);

//         if (!visual)
//         {
//             continue;
//         }

//         pict_format = XRenderFindVisualFormat(gw->display, gw->visual->visual);
//         if (!pict_format)
//         {
//             continue;
//         }

//         gw->fbconfig = gw->fbconfigs[i];
//         if (pict_format->direct.alphaMask > 0)
//         {
//             break;
//         }
//         XFree(visual);
//     }

//     if (!fbconfig)
//     {

//     }

//     // describe_fbconfig

//     gw->colormap = XCreateColormap(gw->display, gw->visual->visual, AllocNone);

//     attr.colormap = gw->colormap;
//     attr.background_pixmap = None;
//     atrr.border_pixmap = None;
//     attr.border_pixel = 0;
//     attr.event_mask = ExposureMask | ButtonPressMask | EnterWindowMask |
//                       LeaveWindowMask |KeyPressMask | StructureNotifyMask;

//     int x = gw->width / 2;
//     int y = gw->height / 2;
//     int attr_mask = CWColormap | CWBorderPixel | CWEventMask;

//     gw->window = XCreateWindow(
//         gw->display,
//         gw->root,
//         x, y,
//         gw->width, gw->height,
//         gw->visual->depth,
//         InputOutput,
//         gw->visual->visual,
//         attr_mask,
//         &attr
//     );

//     int glXattr[] = { None };
//     gw->glx_window = glXCreateWindow(gw->display, gw->fbconfig, gw->window, glXattr);

//     XEvent event;

// 	XSizeHints hints;
//     hints.x = x;
//     hints.y = y;
//     hints.width = gw->width;
//     hints.height = gw->height;
//     hints.flags = USPosition | USSize;

// 	XWMHints *startup_state = XAllocWMHints();
//     startup_state->initial_state = NormalState;
//     startup_state->flags = StateHint;

//     XSetWMProperties(gw->display, gw->window, NULL, NULL, NULL, 0,
//                      &hints, startup_state, NULL);

//     XFree(startup_state);

//     XMapRaised(gw->display, gw->window);
//     XIfEvent(gw->display, &gw->event, WaitForMapNotify, (char *)&gw->window);

//     if ((gw->wm_delete_window = XInternAtom(gw->display, "WM_DELETE_WINDOW", 0)) != None) {
// 		XSetWMProtocols(gw->display, gw->window, &gw->wm_delete_window, 1);
// 	}
// }

// TODO(liam): big todo
// 3D Rendering

void AssimpLoadMesh(Arena *arena, Mesh3D *meshTo, const struct aiMesh *meshFrom)
{
    // TODO(liam): hehe
	for (uint32 i = 0; i < meshFrom->mNumVertices; i++)
	{
		struct aiVector3D meshVertex = meshFrom->mVertices[i];

		/*uint32ArrayAppend(arena, &meshTo->vertices, meshVertex.x);*/
		/*uint32ArrayAppend(arena, &meshTo->vertices, meshVertex.y);*/
		/*uint32ArrayAppend(arena, &meshTo->vertices, meshVertex.z);*/
        ArrayVec3fPush(arena, &meshTo->vertices, (Vector3f){meshVertex.x, meshVertex.y, meshVertex.z});
	}

	for (uint32 i = 0; i < meshFrom->mNumFaces; i++)
	{
		struct aiFace meshFace = meshFrom->mFaces[i];

		ArrayU32Push(arena, &meshTo->faces, meshFace.mIndices[0]);
		ArrayU32Push(arena, &meshTo->faces, meshFace.mIndices[1]);
		ArrayU32Push(arena, &meshTo->faces, meshFace.mIndices[2]);
	}
}

bool32 AssimpLoadAsset(Arena *arena, Mesh3D *meshOut, const char *path)
{
    bool32 result = false;
    const C_STRUCT aiScene *scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene)
    {
        // NOTE(liam): read was successful.
        result = true;

        // NOTE(liam): assume only one mesh for now.
        printf("INFO: number of meshes found: %d\n", scene->mNumMeshes);
        const struct aiMesh *meshIn = scene->mMeshes[0];
        AssimpLoadMesh(arena, meshOut, meshIn);
    }
    else
    {
        printf("ERROR: while loading model '%s'\n", aiGetErrorString());
    }

    aiReleaseImport(scene);
    return result;
}

// TODO(liam): lets just use an opengl context for these.
void buildModelMatrix(Arena *arena, Matrix model, Vector3f position, Vector3f orientation, Vector3f scale)
{
	/*Assert((model.rows == 4) && (model.cols == 4) && "model matrix must be 4x4");*/
	/*Assert((position.cols == 3) && "position must be 1x3");*/
	/*Assert((position.cols == orientation.cols) == scale.cols && "orientation/scale must be 1x3");*/

    ArenaTemp tmp = ArenaScratchCreate(arena);

    MatrixIdentity(model);

    Matrix matScale     = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matTranslate = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matRotYaw    = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matRotPitch  = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matRotRoll   = MatrixIdentity(Matrix4DAlloc(arena));

    MatrixAT(matScale, 0, 0) = scale.x;
    MatrixAT(matScale, 1, 1) = scale.y;
    MatrixAT(matScale, 2, 2) = scale.z;

    MatrixAT(matRotYaw, 0, 0) = cosf(orientation.y);
    MatrixAT(matRotYaw, 0, 2) = sinf(orientation.y);
    MatrixAT(matRotYaw, 2, 2) = cosf(orientation.y);
    MatrixAT(matRotYaw, 2, 0) = -1 * sinf(orientation.y);

    MatrixAT(matRotPitch, 1, 1) = cosf(orientation.x);
    MatrixAT(matRotPitch, 1, 2) = -1 * sinf(orientation.x);
    MatrixAT(matRotPitch, 2, 2) = cosf(orientation.x);
    MatrixAT(matRotPitch, 2, 0) = sinf(orientation.x);

    MatrixAT(matRotRoll, 0, 0) = cosf(orientation.z);
    MatrixAT(matRotRoll, 0, 1) = -1 * sinf(orientation.z);
    MatrixAT(matRotRoll, 1, 1) = cosf(orientation.z);
    MatrixAT(matRotRoll, 1, 0) = sinf(orientation.z);

    MatrixAT(matTranslate, 3, 0) = position.x;
    MatrixAT(matTranslate, 3, 1) = position.y;
    MatrixAT(matTranslate, 3, 2) = position.z;

    MatrixDot_(model, matScale, model);
    MatrixDot_(model, matRotYaw, model);
    MatrixDot_(model, matRotPitch, model);
    MatrixDot_(model, matRotRoll, model);
    MatrixDot_(model, matTranslate, model);

    ArenaScratchFree(tmp);
}

void GDrawMesh(Arena *arena, GWin *gw, Matrix model, Matrix view, Matrix projection,
        Mesh3D mesh, uint32 color_pixel)
{
    ArenaTemp tmp = ArenaScratchCreate(arena);

    Matrix mvp = Matrix4DAlloc(arena);
    Row localA = RowArenaAlloc(arena, 4);
    Row localB = RowArenaAlloc(arena, 4);
    Row localC = RowArenaAlloc(arena, 4);

    Row clipA = RowArenaAlloc(arena, 4);
    Row clipB = RowArenaAlloc(arena, 4);
    Row clipC = RowArenaAlloc(arena, 4);

    MatrixDot_(mvp, projection, view);
    MatrixDot_(mvp, mvp, model);

    for (uint32 i = 0; i < mesh.faces.size; i += 3)
    {
        // TODO(liam): should be 3D, but what type?
        Vector3f vertA = mesh.vertices.V[mesh.faces.V[i]];
        Vector3f vertB = mesh.vertices.V[mesh.faces.V[i + 1]];
        Vector3f vertC = mesh.vertices.V[mesh.faces.V[i + 2]];

        /*Vector3fToRow4D(localA, vertA);*/
        RowAT(localA, 0) = vertA.x;
        RowAT(localA, 1) = vertA.y;
        RowAT(localA, 2) = vertA.z;
        RowAT(localA, 3) = 1.0f;
        /*Vector3fToRow4D(localB, vertB);*/
        RowAT(localB, 0) = vertB.x;
        RowAT(localB, 1) = vertB.y;
        RowAT(localB, 2) = vertB.z;
        RowAT(localB, 3) = 1.0f;
        /*Vector3fToRow4D(localC, vertC);*/
        RowAT(localC, 0) = vertC.x;
        RowAT(localC, 1) = vertC.y;
        RowAT(localC, 2) = vertC.z;
        RowAT(localC, 3) = 1.0f;

        MatrixDot_(clipA, localA, mvp);
        MatrixDot_(clipB, localB, mvp);
        MatrixDot_(clipC, localC, mvp);

        MatrixMulS_(clipA, clipA, 1.f / RowAT(clipA, 3));
        MatrixMulS_(clipB, clipB, 1.f / RowAT(clipB, 3));
        MatrixMulS_(clipC, clipC, 1.f / RowAT(clipC, 3));

        float ndcA_x = RowAT(clipA, 0) / RowAT(clipA, 3);
        float ndcA_y = RowAT(clipA, 1) / RowAT(clipA, 3);

        float ndcB_x = RowAT(clipB, 0) / RowAT(clipB, 3);
        float ndcB_y = RowAT(clipB, 1) / RowAT(clipB, 3);

        float ndcC_x = RowAT(clipC, 0) / RowAT(clipC, 3);
        float ndcC_y = RowAT(clipC, 1) / RowAT(clipC, 3);

        // Convert from NDC to screen space
        Vector2u screenA = {
            (uint32)((ndcA_x + 1.0f) * 0.5f * gw->width),
            (uint32)((1.0f - ndcA_y) * 0.5f * gw->height)
        };

        Vector2u screenB = {
            (uint32)((ndcB_x + 1.0f) * 0.5f * gw->width),
            (uint32)((1.0f - ndcB_y) * 0.5f * gw->height)
        };

        Vector2u screenC = {
            (uint32)((ndcC_x + 1.0f) * 0.5f * gw->width),
            (uint32)((1.0f - ndcC_y) * 0.5f * gw->height)
        };

        // clip to screen
        // i.e, viewport and 3d
        /*Vector2u screenA = ClipToScreen(...);*/

        GDrawTriangle(
            gw,
            screenA, screenB, screenC,
            color_pixel
        );
    }

    ArenaScratchFree(tmp);
}
