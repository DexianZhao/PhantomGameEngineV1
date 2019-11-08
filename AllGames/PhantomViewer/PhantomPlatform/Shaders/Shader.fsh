//
//  Shader.fsh
//  PhantomPlatform
//
//  Created by cosmic-life on 14-1-18.
//  Copyright (c) 2014年 Phantom Game Studios. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
