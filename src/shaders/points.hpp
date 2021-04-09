
namespace points {
struct Point {
  vec2 position;
  vec2 velocity;
};

mat3x3 vertex_transform_2d(float width, float height) {
  return mat3x3{{{{2.f / width, 0.f, 0.f}}, {{0.f, -2.f / height, 0.f}}, {{-1.f, 1.f, 1.f}}}};
}

// Shader sources
static const char* const vertex_shader_text = R"#(
    #version 330 core
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec2 velocity;

    uniform mat3 transform;
    uniform float pointSize;
    uniform float maxSpeedSquared;

    out vec4 pointColor;

    float two_pi = 6.2831853072;

    vec3 rgb_from_hsb(in vec3 c){
        vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                                 6.0)-3.0)-1.0,
                         0.0,
                         1.0 );
        rgb = rgb*rgb*(3.0-2.0*rgb);
        return c.z * mix(vec3(1.0), rgb, c.y);
    }

    float mag_2 = pow(velocity.x, 2) + pow(velocity.y, 2);

    float a = atan(velocity.y, velocity.x);
    void main() {
        pointColor = vec4(rgb_from_hsb(vec3(a/two_pi, 1 - (mag_2 / maxSpeedSquared), 1.0)), 1.0);
        gl_PointSize = pointSize;
        gl_Position = vec4(transform * vec3(position, 1.0), 1.0);
    })#";

static const char* const fragment_shader_text = R"#(
    #version 330 core
    out vec4 frag_colour;

    in vec4 pointColor;

    void main() {
        frag_colour = pointColor;
    })#";

}  // namespace points