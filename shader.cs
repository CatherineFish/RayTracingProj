#version 430 core

#define REFRACTION_PROBABILITY 0.8
#define MAX_BOUNCES 6
#define PI 3.14159265358979323846
#define FOV (5*PI/9)
#define BSIZE .12

struct sphere {
    vec3 center;
    float radius;
};

struct plane {
    vec3 normal;
    float base;
};

struct ray {
    vec4 origin;
    vec4 dir;
};

struct cylinder {
    vec3 center;
    float radius;
    float height;
};

struct circle {
    vec3 center;
    vec3 normal;
    float radius;
};

struct material {
    vec4 color;
    bool glass;
};

layout(local_size_x = 1, local_size_y = 1) in;

layout(location = 0, binding = 0, rgba32f) uniform image2D out_img;
layout(location = 1, binding = 1) uniform sampler2DArray textures;

layout(location = 2) uniform int frame;

uvec4 state = uvec4(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.yx*1664525U)*1664525U*(frame+1);


bool sp_intersect(in sphere sp, in ray r, out vec4 point, out vec4 normal) {
    vec4 oc = r.origin - vec4(sp.center, 1);
    float b = dot(oc, r.dir);
    float c = dot(oc, oc) - sp.radius*sp.radius;
    float d = sqrt(b*b - c);
    float t0 = d - b, t1 = -d - b;
    if (t0 > 1e-4) {
        if (t1 > 1e-4) t0 = t1;
        point = r.origin + t0 * r.dir;
        normal = normalize(point - vec4(sp.center, 1));
        return true;
    }
    return false;
}

bool pl_intersect(in plane pl, in ray r, out vec4 point, out vec4 normal) {
    float pr = dot(pl.normal, r.dir.xyz);
    float b = (pl.base - dot(r.origin.xyz, pl.normal));
    if (abs(pr) > 1e-6 && b/pr > 0) {
        point = r.origin + (b / pr) * r.dir;
        normal = vec4(pl.normal, 0);
        return true;
    }
    return false;
}

int bx_intersect(in plane p[6], in ray r, out vec4 point) {
    int any_pt = -1;
    vec4 pt, normal;
    for (int i = 0; i < p.length(); i++) {
        if (!pl_intersect(p[i], r, pt, normal)) continue;

        bool accepted = true;
        for (int j = 0; j < p.length(); j++) {
            if (i != j && dot(p[j].normal, pt.xyz) - p[j].base > 1e-4) {
                accepted = false;
                break;
            }
        }
        if (!accepted) continue;
        if (any_pt < 0 || distance(r.origin, point) > distance(r.origin, pt)) {
            any_pt = i;
            point = pt;
        }
    }
    return any_pt;
}

bool cyl_intersect(in cylinder sp, in ray r, out vec4 point, out vec4 normal) {
    vec2 oc = r.origin.xy - sp.center.xy;
    vec2 dir = normalize(r.dir.xy);
    float b = dot(oc, dir);
    float c = dot(oc, oc) - sp.radius*sp.radius;
    float d = sqrt(b*b - c);
    float t0 = d - b, t1 = -d - b;
    if (t0 > 1e-4) {
        if (t1 > 1e-4) t0 = t1;
        point = r.origin + t0 * r.dir/length(r.dir.xy);
        if (sp.center.z < point.z && point.z < sp.center.z + sp.height) {
            normal = normalize(vec4(point.xyz - sp.center, 0));
            return true;
        }
        return false;
    }
    return false;
}


bool disk_intersect(in circle cl, in ray r, out vec4 point, out vec4 normal) {
    plane pl = {cl.normal, dot(cl.normal, cl.center)};
    if (pl_intersect(pl, r, point, normal)) {
        if (distance(cl.center, point.xyz) > cl.radius) return false;
        return true;
    }
    return false;
}

bool intersect_block(in ray r, out vec4 point, out vec4 norm, out material color1, in vec4 center, in int btex[6]) {
    r.origin -= center;
    plane b[6] = {
        {{1, 0, 0}, BSIZE/2},
        {{-1, 0, 0}, BSIZE/2},
        {{0, 1, 0}, BSIZE/2},
        {{0, -1, 0}, BSIZE/2},
        {{0, 0, 1}, BSIZE/2},
        {{0, 0, -1}, BSIZE/2},
    };
    int bi = bx_intersect(b, r, point);
    if (bi >= 0) {
        vec3 d = point.xyz - b[bi].normal*b[bi].base;
        vec3 ux = b[(bi + 2) % 6].normal;
        vec3 vx = b[(bi + 4) % 6].normal;
        norm = vec4(b[bi].normal, 0);
        if (bi < 4) {
            vx = vec3(0, 0, -1);
            ux = cross(norm.xyz, vx);
        }
        point += center;
        vec3 uv = vec3((dot(d, ux) + BSIZE/2)/BSIZE, (dot(d, vx) + BSIZE/2)/BSIZE, btex[bi]);
        color1 = material(texture(textures, uv), false);
        if (btex[bi] == 0 || btex[bi] == 44) color1.color *= vec4(0, 1, 0, 0);
        return true;
    }
    return false;
}

bool intersect(in ray r, out vec4 ipoint1, out vec4 inorm1, out material color1) {
    sphere s = {vec3(0,0,.1), 1};
    plane p = {vec3(0,0,1), -1.2};
    circle cl = {vec3(0, 0, -0.8), vec3(0, 0, 1), .8};
    cylinder cy = {vec3(0, 0, -1), .8, 0.2};
    cylinder cy2 = {vec3(0, 0, -1.2), 1, 0.2};
    circle cl2 = {vec3(0, 0, -1), vec3(0, 0, 1), 1};
    bool intr_any = false;
    vec4 inorm2, ipoint2;
    material cyl_material = material(vec4(.4, 1, .4, 1), false);
    if (cyl_intersect(cy, r, ipoint2, inorm2)) {
        if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
            ipoint1 = ipoint2;
            inorm1 = inorm2;
            color1 = cyl_material;
        }
        intr_any = true;
    }
    if (disk_intersect(cl, r, ipoint2, inorm2)) {
        if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
            ipoint1 = ipoint2;
            inorm1 = inorm2;
            color1 = cyl_material;
        }
        intr_any = true;
    }
    if (cyl_intersect(cy2, r, ipoint2, inorm2)) {
        if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
            ipoint1 = ipoint2;
            inorm1 = inorm2;
            color1 = cyl_material;
        }
        intr_any = true;
    }
    if (disk_intersect(cl2, r, ipoint2, inorm2)) {
        if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
            ipoint1 = ipoint2;
            inorm1 = inorm2;
            color1 = cyl_material;
        }
        intr_any = true;
    }
    if (sp_intersect(s, r, ipoint2, inorm2)) {
        if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
            ipoint1 = ipoint2;
            inorm1 = inorm2;
            color1 = material(vec4(1, 1, 1, 1), true);
        }
        intr_any = true;
    }
    if (pl_intersect(p, r, ipoint2, inorm2)) {
        if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
            ipoint1 = ipoint2;
            inorm1 = inorm2;
            color1 = material(texture(textures, vec3(ipoint2.xy*2, 30)), false);
        }
        intr_any = true;
    }

    return intr_any;
}

bool intersect_blocks(in ray r, out vec4 ipoint1, out vec4 inorm1, out material color1) {
    int btex[6][6] = {
        {3,3,3,3,0,2},
        {2,2,2,2,2,2},
        {23,23,24,23,22,22},
        {17,17,17,17,18,18},
        {44,44,44,44,44,44},
        {46,50,46,50,37,37},
    };
    int blocks[3*4*9] = {
        1,1,1,
        1,1,1,
        1,1,1,
        1,1,1,
        0,0,0,
        0,0,0,
        0,0,0,
        0,0,0,
        -1,-1,-1,
        -1, 3, 5,
        -1, 2, -1,
        -1, -1, -1,
        -1,-1,-1,
        -1, 3, -1,
        -1, -1, -1,
        -1, -1, -1,
        -1,-1,-1,
        -1, 3, -1,
        -1, -1, -1,
        -1, -1, -1,
        4,4,4,
        4, 3, 4,
        4, 4, 4,
        -1, -1, -1,
        -1,4,-1,
        4, 4, 4,
        -1, 4, -1,
        -1, -1, -1,
        -1,-1,-1,
        -1, 4, -1,
        -1, -1, -1,
        -1, -1, -1,
        -1,-1,-1,
        -1, 4, -1,
        -1, -1, -1,
        -1, -1, -1,
    };
    bool intr_any = false;
    vec4 inorm2, ipoint2;
    material m2;
    for (int x = -1; x <= 1; x++) {
        for (int y = -2; y <= 1; y++) {
            for (int z = -1; z <= 7; z++) {
                int block = blocks[1+x+3*(2+y)+12*(1+z)];
                if (block >= 0 && intersect_block(r, ipoint2, inorm2, m2, vec4(x, y, z-2, 0)*BSIZE, btex[block])) {
                    if (!intr_any || distance(ipoint1, r.origin) > distance(ipoint2, r.origin)) {
                        ipoint1 = ipoint2;
                        inorm1 = inorm2;
                        color1 = m2;
                    }
                    intr_any = true;
                }
            }
        }
    }
    return intr_any;
}

float fresnel_shlick(float f0, float ndotl) {
    return f0 + (1 - f0)*pow(1 - ndotl, 5);
}

#define GLASS_IOR 1.517
#define F0 (GLASS_IOR - 1)*(GLASS_IOR - 1)/((GLASS_IOR + 1)*(GLASS_IOR + 1))

vec4 refraced_weight(vec4 n, vec4 l, vec4 v, material m) {
    return vec4(1 - fresnel_shlick(F0, abs(dot(n, l))));
}

vec4 direct_weight(vec4 n, vec4 l, vec4 v, material m) {
    vec4 c = vec4(0, 0, 0, 0);
    if (m.glass) {
        c += vec4(1, 1, 1, 0)*min(1, pow(max(0, dot(n, l)), 5)*fresnel_shlick(F0, max(0, dot(n, l)))*32);
        c += m.color*max(0, dot(n, l))/2;
    } else c += m.color*(vec4(1 + max(0, dot(n, l))));
    return c;
}


vec4 reflected_weight(vec4 n, vec4 l, vec4 v, material m) {
    return vec4(fresnel_shlick(F0, max(0, dot(n, l))));
}

float rand() {
    state.x = ((state.x & 4294967294U) << 12) ^ (((state.x << 13) ^ state.x) >> 19);
    state.y = ((state.y & 4294967288U) << 4) ^ (((state.y << 2) ^ state.y) >> 25);
    state.z = ((state.z & 4294967280U) << 17) ^ (((state.z << 3) ^ state.z) >> 11);
    state.w = state.w * 1664525U + 1013904223U;
    return 2.3283064365387e-10 * (state.x ^ state.y ^ state.z ^ state.w);
}


mat3x4 cam_matrix(in vec4 from, in vec4 to, float width, float height) {
    vec4 forward = normalize(to - from);
    vec4 right = normalize(vec4(cross(forward.xyz, vec3(0, 0, 1)), 0));
    vec4 up = normalize(vec4(cross(right.xyz, forward.xyz), 0));
    vec4 base = forward*cos(FOV/2) - (up*(height/width) + right)*.5;
    return mat3x4(right/height, up/height, base);
}

void main() {
    vec4 color = vec4(0);
    ivec3 pos = ivec3(gl_GlobalInvocationID.xy, 1);
    ivec2 sz = imageSize(out_img);

    vec4 origin = vec4(1, 2.6, 1.3, 1);
    mat3x4 cam = cam_matrix(origin, vec4(0, 0, 0, 1), sz.x, sz.y);

    vec3 p = vec3(pos) + vec3(rand() -.5, rand() - .5, 0);
    ray r = { origin, normalize(cam*pos) };

    vec4 lights[] = {
        vec4(-2, 3, 1.6, 1),
        vec4(2.7, 2, 1.4, 1),
    };
    vec4 intensity[] = {
        3*vec4(1, 0, 1, 1),
        3*vec4(1, 1, 1, 1),
    };
    vec4 weight = vec4(1);

    vec4 i_point, i_normal, i_point2, i_normal2;
    material m, m2;
    bool inside = false;
    for (int i = 0; i < MAX_BOUNCES; i++) {
        bool binstrs = inside && intersect_blocks(r, i_point, i_normal, m);
        if (binstrs) {
            float alpha = min(1, exp(-length(i_point/5)));
            for (int i = 0; i < lights.length(); i++) {
                vec4 l = lights[i] - i_point;
                ray to_light = {i_point + i_normal*1e-6, normalize(l)};
                if (dot(i_normal, r.dir) > 0) i_normal = -i_normal;
                color += intensity[i]*weight*direct_weight(i_normal, normalize(l), -r.dir, m)/dot(l, l)*alpha;
            }
            weight *= 1-alpha;
        }
        if (intersect(r, i_point, i_normal, m)) {
            for (int i = 0; i < lights.length(); i++) {
                vec4 l = lights[i] - i_point;
                ray to_light = {i_point + i_normal*1e-6, normalize(l)};
                if (dot(i_normal, r.dir) > 0) i_normal = -i_normal;
                if (binstrs || !intersect(to_light, i_point2, i_normal2, m2))
                    color += intensity[i]*weight*direct_weight(i_normal, normalize(l), -r.dir, m)/dot(l, l);
            }

            vec4 l;
            if (m.glass) {
                 if (rand() < REFRACTION_PROBABILITY) {
                     i_point += i_normal*1e-6;
                     l = reflect(r.dir, i_normal);
                     weight *= reflected_weight(i_normal, normalize(l), -r.dir, m)/(2*(1 - REFRACTION_PROBABILITY));
                 } else {
                     float ior = GLASS_IOR;
                     if (!inside) ior = 1/ior;
                     inside = !inside;
                     i_point += i_normal*1e-6;
                     l = refract(r.dir, i_normal, ior);
                     if (dot(l , l) < 1e-6) {
                         color = vec4(1,0,0,0);
                         break;
                     }
                     weight *= refraced_weight(i_normal, normalize(l), -r.dir, m)/(.35*REFRACTION_PROBABILITY);
                 }
                 r = ray(i_point, l);
            } else break;
        } else {
            color += weight*mix(vec4(0, 1, 1, 1), vec4(0, 0.4, 1, 1), .5*(r.dir.z + 1));
            break;
        }
    }

    vec4 c = imageLoad(out_img, pos.xy);
    color = (c*frame + color)/(frame + 1);
    imageStore(out_img, pos.xy, color);
}
