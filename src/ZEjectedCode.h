/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2014 Laurent Thiebaut & Olivia Merle
 *
 * Blackvoxel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blackvoxel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * ZEjectedCode.h
 *
 *  Created on: 14 mai 2011
 *      Author: laurent
 */

#pragma once

#include "SDL2/SDL_Keyboard.h"

void EjectedCode
{
  // Texture Loading
  img.LoadBMP("/home/laurent/test/test_2.bmp");
  img2.LoadBMP("/home/laurent/test/test_3.bmp");
  img3.LoadBMP("/home/laurent/test/test_4.bmp");
  img4.LoadBMP("textures/texture_cubeglow.bmp");
  img4_m1.LoadBMP("textures/texture_cubeglow_mip_1.bmp");
  img4_m2.LoadBMP("texturSetStylees/texture_cubeglow_mip_2.bmp");
  img4_m3.LoadBMP("textures/texture_cubeglow_mip_3.bmp");
  img4_m4.LoadBMP("textures/texture_cubeglow_mip_4.bmp");
  img4_m5.LoadBMP("textures/texture_cubeglow_mip_5.bmp");
  img4_m6.LoadBMP("textures/texture_cubeglow_mip_6.bmp");
  img4_m7.LoadBMP("textures/texture_cubeglow_mip_7.bmp");
  img4_m8.LoadBMP("textures/texture_cubeglow_mip_8.bmp");
  img4_m9.LoadBMP("textures/texture_cubeglow_mip_9.bmp");


  glGenTextures(1,&TextureName[0]);
   glBindTexture(GL_TEXTURE_2D,TextureName[0]);
  glTexImage2D ( GL_TEXTURE_2D,      //Type : texture 2D
                 0,  //Mipmap : aucun
                 GL_RGBA8,   //Couleurs : 4
                 img.Width,        //Largeur : 2
                 img.Height,         //Hauteur : 2
                 0,  //Largeur du bord : 0     img4_m6.LoadBMP("textures/texture_cubeglow_mip_6.bmp");

                 GL_BGRA,    //Format : RGBA
                 GL_UNSIGNED_BYTE,   //Type des couleurs
                 (GLubyte *)img.BitmapMemory         //Addresse de l'image
               );
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR GL_NEAREST
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);



 glGenTextures(1,&TextureName[1]);
 glBindTexture(GL_TEXTURE_2D,TextureName[1]);
  glTexImage2D ( GL_TEXTURE_2D,      //Type : texture 2D
                 0,  //Mipmap : aucun
                 GL_RGBA8,   //Couleurs : 4
                 img.Width,  //Largeur : 2
                 img.Height,         //Hauteur : 2
                 0,  //Largeur du bord : 0
                 GL_BGRA,    //Format : RGBA
                 GL_UNSIGNED_BYTE,   //Type des couleurs
                 (GLubyte *)img2.BitmapMemory        //Addresse de l'image
               );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);


  glGenTextures(1,&TextureName[2]);
  glBindTexture(GL_TEXTURE_2D,TextureName[2]);
  glTexImage2D ( GL_TEXTURE_2D,      //Type : texture 2D
                 0,  //Mipmap : aucun
                 GL_RGBA8,   //Couleurs : 4
                 img.Width,  //Largeur : 2
                 img.Height,         //Hauteur : 2
                 0,  //Largeur du bord : 0
                 GL_BGRA,    //Format : RGBA
                 GL_UNSIGNED_BYTE,   //Type des couleurs
                 (GLubyte *)img3.BitmapMemory        //Addresse de l'image
               );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

  glGenTextures(1,&TextureName[3]);
  glBindTexture(GL_TEXTURE_2D,TextureName[3]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /*

  glTexImage2D ( GL_TEXTURE_2D,      //Type : texture 2D
                 1,  //Mipmap : aucun
                 GL_RGBA8,   //Couleurs : 4
                 img5.Width,         //Largeur : 2
                 img5.Height,        //Hauteur : 2
                 0,  //Largeur du bord : 0
                 GL_BGRA,    //Format : RGBA
                 GL_UNSIGNED_BYTE,   //Type des couleurs
                 (GLubyte *)img5.BitmapMemory        //Addresse de l'image
               );
               */
  glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA8, img4.Width,  img4.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 1, GL_RGBA8, img4_m1.Width,  img4_m1.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m1.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 2, GL_RGBA8, img4_m2.Width,  img4_m2.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m2.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 3, GL_RGBA8, img4_m3.Width,  img4_m3.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m3.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 4, GL_RGBA8, img4_m4.Width,  img4_m4.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m4.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 5, GL_RGBA8, img4_m5.Width,  img4_m5.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m5.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 6, GL_RGBA8, img4_m6.Width,  img4_m6.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m6.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 7, GL_RGBA8, img4_m7.Width,  img4_m7.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m7.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 8, GL_RGBA8, img4_m8.Width,  img4_m8.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m8.BitmapMemory );
  glTexImage2D ( GL_TEXTURE_2D, 9, GL_RGBA8, img4_m9.Width,  img4_m9.Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4_m9.BitmapMemory );

  //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, img4.Width, img4.Height,  GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte *)img4.BitmapMemory );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);





  // Shaders

/*   GLchar * VertexShader ="void main()\n"
                           "{\n"
                                   "  gl_Position = ftransform();\n"
                           "}\n";*/

GLchar * VertexShader =(GLchar *)
                       "attribute vec4 aTranslation;\n"
                       "void main()\n"
                               "{"
                               "  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (gl_Vertex + aTranslation) ;\n" // * aTranslation
                               "  gl_TexCoord[0] = gl_MultiTexCoord0;\n"
                       "  gl_TexCoord[1] = gl_MultiTexCoord1;\n"
                               "  gl_FrontColor = gl_Color;\n"
                       "}";


GLchar * PixelShader = (GLchar *)
                       " uniform sampler2D tex;\n"
                               " void main()\n"
                               " {\n"
                       "  gl_FragColor =  texture(tex, gl_TexCoord[0].xy );\n" // gl_FragCoord.xy
                               "  //gl_FragColor = gl_Color; \n" //vec4 (noise1(0),0.4,0.8,1.0);
                               " }\n";



  int status;
  vsh = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsh, 1, (const GLchar **)&VertexShader, 0);
  glCompileShader(vsh);
  glGetShaderiv(vsh, GL_COMPILE_STATUS, &status );
  if (status!=GL_TRUE) { printf("Vertex shader compilation failled\n"); exit(0); }
  else                 printf("Vertex shader compilation OK\n");

  psh = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(psh, 1, (const GLchar **)&PixelShader, 0);
  glCompileShader(psh);
  glGetShaderiv(psh, GL_COMPILE_STATUS, &status );
  if (status!=GL_TRUE) { printf("Pixel shader compilation failled\n"); exit(0); }
  else                 printf("Pixel shader compilation OK\n");

  pr = glCreateProgram();
  glAttachShader(pr, vsh);
  glAttachShader(pr, psh);
  glLinkProgram(pr);
  glGetProgramiv(pr, GL_LINK_STATUS, &status);
  if (status!=GL_TRUE) printf("Shader link failled\n");
  else                 printf("Shader link OK\n");

}


void MouseHandler( int dx, int dy)
{
}


void KeyHandler (ZGame & GameEnv)
{
  ZActor * Actor;
  UByte * keyboardState;

  keyboardState = SDL_GetKeyboardState(NULL);

   if ( keyboardState[SDL_SCANCODE_Z] ) {Player.x += sin(Player.Yaw/180 * 3.14159265)*100; Player.z -=cos(Player.Yaw/180 * 3.14159265)*100; }
   if ( keyboardState[SDL_SCANCODE_S] ) {Player.x -= sin(Player.Yaw/180 * 3.14159265)*100; Player.z +=cos(Player.Yaw/180 * 3.14159265)*100; }


  Actor = GameEnv.PhysicEngine->GetSelectedActor();

  if (Actor)
  {
    if ( keyboardState[SDL_SCANCODE_Q] ) { Actor->Action_GoLeftStraff((double)RenderTime * 1.5); }
    if ( keyboardState[SDL_SCANCODE_D] ) { Actor->Action_GoRightStraff((double)RenderTime  * 1.5); }
    if ( keyboardState[SDL_SCANCODE_Z] ) { Actor->Action_GoForward((double)RenderTime  * 1.5); }
    if ( keyboardState[SDL_SCANCODE_S] ) { Actor->Action_GoBackward((double)RenderTime  * 1.5); }
    if ( keyboardState[SDL_SCANCODESPACE]){Actor->Action_Jump((double)RenderTime * 4.0  * 1.5);}
    if ( keyboardState[SDL_SCANCODE_A] ) { Actor->Action_GoUp((double)RenderTime  * 1.5); }
    if ( keyboardState[SDL_SCANCODE_W] ) { Actor->Action_GoDown((double)RenderTime  * 1.5); }
    if ( keyboardState[SDL_SCANCODEKP_PLUS] ) { Actor->Action_NextBuildingMaterial(); keyboardState[SDL_SCANCODEKP_PLUS] = 0; }
    if ( keyboardState[SDL_SCANCODEKP_MINUS] ) { Actor->Action_PrevBuildingMaterial(); keyboardState[SDL_SCANCODEKP_MINUS] = 0; }
    if ( keyboardState[SDL_SCANCODE_I] )
    {
//      printf("Main Memory SectorCount:%lu\n", World.Info_GetSectorsInMemory()); World.Info_PrintHashStats();

      // Needs GL_NVX_gpu_memory_info extension
      GLint Availlable_VRam = 0;
      // glGetIntegerv(/*GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX*/ 0x9049, &Availlable_VRam);
      GLint Total_Memory = 0;
      glGetIntegerv(/*GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX*/ 0x9048,&Total_Memory);
      printf("GPU MEMORY FREE(Ko): %d/%d\n",Availlable_VRam, Total_Memory);
      keyboardState[SDL_SCANCODE_I]=0;
    }
  }
  if ( keyboardState[SDL_SCANCODE_B] ) {debug_break = true; }
  if ( keyboardState[SDL_SCANCODE_G] ) {Actor->Velocity.y = 15000.0; }
  if ( keyboardState[SDL_SCANCODE_H] ) { keyboardState[SDL_SCANCODE_H]=0; }
  if ( keyboardState[SDL_SCANCODE_K] ) {Actor->Velocity.z += 15000.0; Actor->Velocity.y += 1000.0;keyboardState[SDL_SCANCODE_K] = 0; }
  if ( keyboardState[SDL_SCANCODEF8])
  {
    ELong lx,ly,lz;
    lx = (floor(Actor->Location.x / 256.0 / 16.0));
    ly = (floor(Actor->Location.y / 256.0 / 16.0));
    lz = (floor(Actor->Location.z / 256.0 / 16.0));
    Long f_x,f_y,f_z;

    f_x = lx ;
    f_y = ly -1;
    f_z = lz;

//      World.FindSector( f_x,f_y,f_z )->DebugOutFCInfo("outfc.txt");
//      World.FindSector( f_x,f_y,f_z )->DebugOut("out.txt");
    printf("Sector : %ld %ld %ld : %lf,%lf,%lf\n",f_x,f_y,f_z, floor(Actor->Location.x / 256.0), floor(Actor->Location.y / 256.0), floor(Actor->Location.z / 256.0));

    keyboardState[SDL_SCANCODEF8] = 0;
  }

/*

  if ( keyboardState[SDL_SCANCODEF7])
  {
    ELong lx,ly,lz;
    lx = (floor(Actor->Location.x / 256.0 / 16.0));
    ly = (floor(Actor->Location.y / 256.0 / 16.0));
    lz = (floor(Actor->Location.z / 256.0 / 16.0));
    Long f_x,f_y,f_z;

    f_x = lx ;
    f_y = ly -1;
    f_z = lz;

    ZVoxelSector * Sector = World.FindSector( f_x,f_y,f_z );

    Sector->PartialCulling = 63;
    //World.SectorUpdateFaceCulling(f_x,f_y,f_z);
    printf("Culling: %ld,%ld,%ld\n",f_x,f_y,f_z);
    Sector->Flag_Render_Dirty = true;
    Sector->Flag_Void_Regular = false;
    Sector->Flag_Void_Transparent = false;
    keyboardState[SDL_SCANCODEF7] = 0;
  }
*/


/*
  if ( keyboardState[SDL_SCANCODE_Z] ) {Player.x += sin(Player.Yaw/180 * 3.14159265)*100; Player.z -=cos(Player.Yaw/180 * 3.14159265)*100; }
  if ( keyboardState[SDL_SCANCODE_S] ) {Player.x -= sin(Player.Yaw/180 * 3.14159265)*100; Player.z +=cos(Player.Yaw/180 * 3.14159265)*100; }

  if ( keyboardState[SDL_SCANCODE_Q] ) {Player.x += cos(Player.Yaw/180 * 3.14159265)*100; Player.z +=sin(Player.Yaw/180 * 3.14159265)*100; }
  if ( keyboardState[SDL_SCANCODE_D] ) {Player.x -= cos(Player.Yaw/180 * 3.14159265)*100; Player.z -=sin(Player.Yaw/180 * 3.14159265)*100; }
  if ( keyboardState[SDL_SCANCODE_F] ) {Plf Z_ZSCREEN_SLOTSELECTION_H
  //#  include "ZScree,_SlotSelection.h"
  //#endifayer.y += 100; }
  if ( keyboardState[SDL_SCANCODE_C] ) {Player.y -= 100; }

  if ( keyboardState[SDL_SCANCODE_E] ) {Player.Yaw -= 5.0; }
  if ( keyboardState[SDL_SCANCODE_A] ) {Player.Yaw += 5.0; }

  if ( keyboardState[SDL_SCANCODE_Y] ) {SpecialValue += 1.0; if (SpecialValue >= 360.0) SpecialValue-=360.0; printf("SV1: %f\n",SpecialValue); }
  if ( keyboardState[SDL_SCANCODE_H] ) {SpecialValue -= 1.0; if (SpecialValue <0.0) SpecialValue+=360.0;printf("SV1: %f\n",SpecialValue); }
  if ( keyboardState[SDL_SCANCODE_O] ) {SpecialValue3 += 1; printf("SV3: %ld\n",SpecialValue3); }
  if ( keyboardState[SDL_SCANCODE_L] ) {SpecialValue3 -= 1; printf("SV3: %ld\n",SpecialValue3); }
  if ( keyboardState[SDL_SCANCODE_P] ) {SpecialValue4 += 1; printf("SV4: %ld\n",SpecialValue4); }
  if ( keyboardState[SDL_SCANCODE_M] ) {SpecialValue4 -= 1; printf("SV4: %ld\n",SpecialValue4); }
*/

  if ( keyboardState[SDL_SCANCODE_N] )  { SDL_SetRelativeMouseMode(SDL_FALSE);  }
  if ( keyboardState[SDL_SCANCODEF2] ) { SDL_SetRelativeMouseMode(SDL_TRUE);  }
  if ( keyboardState[SDL_SCANCODEF1] ) { SDL_SetRelativeMouseMode(SDL_FALSE);  }
  //if ( keyboardState[SDL_SCANCODEF5] ) { World.Save(); }
  //if ( keyboardState[SDL_SCANCODEF9] ) { World.Load(); }

  if ( keyboardState[SDL_SCANCODEF12] )  { SDL_SetRelativeMouseMode(SDL_FALSE);  }
  //int fps = 1000 / RenderTime;
  if ( keyboardState[SDL_SCANCODE_R] ) { printf("nCubes : %ld  Render Time: %lu ms Trans : %ld PureRend : %ld x:%lf y:%lf z:%lf \n", nCubesDisplayed, RenderTime, Time_Transform, Time_PureRender, Player.x, Player.y, Player.z );}


  if ( keyboardState[SDL_SCANCODE_B] ) { RenderType = 0; }
  if ( keyboardState[SDL_SCANCODE_V] ) { RenderType = 1; }
  if ( keyboardState[SDL_SCANCODE_I] ) { RenderType = 2; }
  if ( keyboardState[SDL_SCANCODE_U] ) { RenderType = 3; }
  if ( keyboardState[SDL_SCANCODE_J] ) { RenderType = 4; }

  if ( keyboardState[SDL_SCANCODE1] ) { Player.Yaw = 0.0; Player.Pitch = 0.0; Player.Roll  = 0.0; }

}

/*


    GLuint Nom[128];
    char keyboardState[1024];

    bool debug_break = 0;

    float Angle_Turn = 180.0;
    float Angle_Dive = 0;
    float plusz = 0.0;
    float SpecialValue = 0.0;
    float SpecialValue2 = 0.0;
    signed long SpecialValue3 = 0;
    signed long SpecialValue4 = 0;
    long UserX = 1518;
    long UserY = 8600;
    long UserZ = 879;

    ULong RenderTime;
    ULong nCubesDisplayed;
    ULong Time_Transform;
    ULong Time_PureRender;

    int RenderType = 0;

    unsigned long DisplayCubeCount;


    int LastX;
    int LastY;

    GLuint pr;
    GLuint vsh;
    GLuint psh;

    GLuint TextureName[1024];

    int zx=0;
    int zy=0;

*/

/*
      while( SDL_PollEvent(&event) )
      {
        switch( event.type )
        {
          case SDL_KEYDOWN: keyboardState[event.key.keysym.sym] = 1;
                          if (event.key.keysym.sym == SDL_SCANCODE_P) Continue = false;
                            break;
          case SDL_KEYUP: keyboardState[event.key.keysym.sym] = 0;
                            break;
          case SDL_MOUSEMOTION:
                          MouseHandler(event.motion.xrel, event.motion.yrel);
                            break;
          case SDL_MOUSEBUTTONDOWN:
                              Actor->Action_MouseButtonClick(event.button.button - 1);
                            break;
          case SDL_MOUSEBUTTONUP:
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glAlphaFunc(GL_GREATER, 0.2);
            glEnable(GL_ALPHA_TEST);
     Actor->Action_MouseButtonRelease(event.button.button - 1);
                            break;

          case SDL_QUIT: Continue = false;
                         printf("Exit.\n");
                         break;

          default:  break;
        }

      }
*/
