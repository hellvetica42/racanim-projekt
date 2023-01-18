#version 430

// Set up our compute groups
layout(local_size_x=COMPUTE_SIZE_X, local_size_y=COMPUTE_SIZE_Y) in;

// Input uniforms go here if you need them.
// Some examples:
uniform vec2 screen_size;
uniform vec4 factors;
uniform float perceptionDistance;
uniform vec2 mouse;
uniform bool mouseHeldR;
uniform bool mouseHeldL;
//uniform vec2 force;
//uniform float frame_time;

// Structure of the ball data
struct Ball
{
    vec4 pos;
    vec4 vel;
    vec4 color;
};

// Input buffer
layout(std430, binding=0) buffer balls_in
{
    Ball balls[];
} In;

// Output buffer
layout(std430, binding=1) buffer balls_out
{
    Ball balls[];
} Out;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    int curBallIndex = int(gl_GlobalInvocationID);

    Ball in_ball = In.balls[curBallIndex];

    vec4 p = in_ball.pos.xyzw;
    vec4 v = in_ball.vel.xyzw;

    // Move the ball according to the current velocity 
    float speedLimit = 10;

    float coherence = factors.x;
    float separation = factors.y;
    float alignment = factors.z;
    float sep_distance = factors.w;

    float margin = 100;
    float turnFactor = 10;

    float centerX = 0;
    float centerY = 0;
    float numneighbours = 0;
    float numsep = 0;

    vec2 center = vec2(0,0);

    vec2 move = vec2(0,0);

    vec2 avgXY = vec2(0,0);

    p.xy += v.xy;

    // Calculate the new force based on all the other bodies
    for (int i=0; i < In.balls.length(); i++) {

        if(i == curBallIndex){
            continue;
        }

        float dist = distance(In.balls[i].pos.xyzw.xy, p.xy); 

        if(dist < perceptionDistance){
            center.xy += In.balls[i].pos.xyz.xy;

            avgXY += In.balls[i].vel.xyz.xy;

            numneighbours += 1;
        }
        if(dist < sep_distance){
            numsep += 1;
            move.xy += p.xy - In.balls[i].pos.xyz.xy;
        }
    }
    if(numneighbours > 0){
        center = center / numneighbours;
        avgXY = avgXY / numneighbours;

        v.xy += (center - p.xy) * coherence;


        v.xy += (avgXY - v.xy) * alignment;

    }
    if(numsep > 0){
        move = move / numsep;
        v.xy += move * separation;
    }

    if(p.x < 0){
        p.x = screen_size.x;
    }

    if(p.x > screen_size.x){
        p.x = 0;
    }

    if(p.y < 0){
        p.y = screen_size.y;
    }

    if(p.y > screen_size.y){
        p.y = 0;
    }

    //v.x += rand(vec2(curBallIndex, v.x));
    //v.y += rand(vec2(curBallIndex, v.y));

    if(mouseHeldL){
        v.xy += normalize(mouse.xy - p.xy) * 5;
    }

    if(mouseHeldR){
        v.xy -= normalize(mouse.xy - p.xy) * 5;
    }

    float speed = length(v);
    if(speed > speedLimit){
        v.x = (v.x / speed) * speedLimit;
        v.y = (v.y / speed) * speedLimit;
    }


    Ball out_ball;
    out_ball.pos.xyzw = p.xyzw;
    out_ball.vel.xyzw = v.xyzw;

    vec4 c = in_ball.color.xyzw;
    out_ball.color.xyzw = normalize(v.xyzw) + vec4(0.5);

    Out.balls[curBallIndex] = out_ball;
}