#version 120

// Включаем расширения для целочисленных операций
#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_shader_integer_mix : enable

uniform sampler2D vramTexture;
uniform vec2 vramSize;
uniform vec4 tpageInfo;
uniform vec4 clutInfo;
uniform vec4 texWindow;

varying vec2 vTexCoord;
varying vec4 vColor;

// Заменяем uint на float для совместимости
vec3 psxColorToRGB(float color15) {
    float r = floor(mod(color15, 32.0));
    float g = floor(mod(color15 / 32.0, 32.0));
    float b = floor(mod(color15 / 1024.0, 32.0));
    return vec3(r, g, b) / 31.0;
}

// Альтернативная реализация без битовых операций
vec3 psxColorToRGB_alt(float color15) {
    // Распаковываем 15-bit цвет используя математику
    float r = mod(color15, 32.0);
    float g = mod(floor(color15 / 32.0), 32.0);
    float b = floor(color15 / 1024.0);
    return vec3(r, g, b) / 31.0;
}

float getColor15(vec2 coord) {
    vec4 texData = texture2D(vramTexture, coord);
    // Комбинируем R и G компоненты в 16-bit значение
    float low = texData.r * 255.0;
    float high = texData.g * 255.0;
    return low + high * 256.0;
}

vec2 applyTextureWindow(vec2 texCoord) {
    vec2 result = texCoord;
    
    if(texWindow.z > 0.0 && texWindow.w > 0.0) {
        result = texWindow.xy + mod(texCoord * texWindow.zw, texWindow.zw);
    }
    
    return result;
}

float getColorIndex(vec2 texCoord, float colorDepth) {
    vec2 finalCoord = applyTextureWindow(texCoord);
    vec2 vramCoord = (vec2(tpageInfo.x, tpageInfo.y) + finalCoord) / vramSize;
    vec4 texData = texture2D(vramTexture, vramCoord);
    
    if(colorDepth == 0.0) { // 4-bit
        float texelValue = texData.r * 255.0;
        float pixelIndex = mod(floor(finalCoord.x), 2.0);
        if(pixelIndex == 0.0) {
            return mod(texelValue, 16.0);
        } else {
            return floor(texelValue / 16.0);
        }
    }
    else { // 8-bit
        return texData.r * 255.0;
    }
}

vec3 getColorFromCLUT(float colorIndex, float clutX, float clutY) {
    vec2 clutCoord = vec2(
        (clutX + colorIndex + 0.5) / vramSize.x,
        (clutY + 0.5) / vramSize.y
    );
    
    float color15 = getColor15(clutCoord);
    return psxColorToRGB(color15);
}

void main() {
    float colorDepth = clutInfo.w;
    vec3 finalColor;
    
    if(colorDepth == 2.0) { // 16-bit direct
        vec2 finalCoord = applyTextureWindow(vTexCoord);
        vec2 vramCoord = (vec2(tpageInfo.x, tpageInfo.y) + finalCoord) / vramSize;
        float color15 = getColor15(vramCoord);
        finalColor = psxColorToRGB(color15);
    } else {
        float colorIndex = getColorIndex(vTexCoord, colorDepth);
        finalColor = getColorFromCLUT(colorIndex, clutInfo.x, clutInfo.y);
    }
    
    gl_FragColor = vColor;   // vec4(finalColor, 1.0) * vColor;
}
