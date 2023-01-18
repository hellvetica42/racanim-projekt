"""
Compute shader with buffers
"""
import random
from array import array

import arcade
from arcade.gl import BufferDescription
from arcade.experimental.uislider import UISlider
from arcade.gui import UIManager, UIAnchorWidget, UILabel
from arcade.gui.events import UIOnChangeEvent

# Window dimensions
WINDOW_WIDTH = 2300
WINDOW_HEIGHT = 1300

# Size of performance graphs
GRAPH_WIDTH = 200
GRAPH_HEIGHT = 120
GRAPH_MARGIN = 5


class MyWindow(arcade.Window):

    def __init__(self):
        super().__init__(WINDOW_WIDTH, WINDOW_HEIGHT,
                         "Compute Shader",
                         gl_version=(4, 3),
                         resizable=True)
        self.center_window()


        self.num_balls = 40000

        self.group_x = 256
        self.group_y = 1

        self.x, self.y = 0, 0
        self.mouseHeldR = False
        self.mouseHeldL = False

        screen_size = self.get_size()
        self.sliderW = 300
        self.sliderH = 50

        self.manager = UIManager()
        self.manager.enable()

        self.coherence = 0.005
        self.separation = 0.3
        self.alignment = 0.05
        self.perception = 500
        self.sep_distance = 30

        coh_slider= UISlider(value=self.coherence*1000, width=self.sliderW, height=self.sliderH)
        coh_label = UILabel(text=f'Coherence: {self.coherence:.5f}')

        sep_slider= UISlider(value=self.separation*100, width=self.sliderW, height=self.sliderH)
        sep_label = UILabel(text=f'Separation: {self.separation:.5f}')

        align_slider= UISlider(value=self.alignment*1000, width=self.sliderW, height=self.sliderH)
        align_label = UILabel(text=f'Alignment: {self.alignment:.5f}')

        percept_slider= UISlider(value=self.perception/10, width=self.sliderW, height=self.sliderH)
        percept_label = UILabel(text=f'PerceptionDistance: {self.perception}')

        sep_distance_slider= UISlider(value=self.sep_distance, width=self.sliderW, height=self.sliderH)
        sep_distance_label = UILabel(text=f'Separation Distance: {self.sep_distance}')

        @coh_slider.event()
        def on_change(event: UIOnChangeEvent):
            coh_label.text = f"Coherence: {coh_slider.value/1000:.5f}"
            coh_label.fit_content()
            self.coherence = coh_slider.value/1000

        @sep_slider.event()
        def on_change(event: UIOnChangeEvent):
            sep_label.text = f"Separation: {sep_slider.value/100:.5f}"
            sep_label.fit_content()
            self.separation = sep_slider.value/100

        @align_slider.event()
        def on_change(event: UIOnChangeEvent):
            align_label.text = f"Alignment: {align_slider.value/1000:.5f}"
            align_label.fit_content()
            self.coherence = align_slider.value/1000

        @percept_slider.event()
        def on_change(event: UIOnChangeEvent):
            percept_label.text = f"PerceptionDistance: {int(percept_slider.value*10)}"
            percept_label.fit_content()
            self.perception = percept_slider.value*10

        @sep_distance_slider.event()
        def on_change(event: UIOnChangeEvent):
            sep_distance_label.text = f"Separation Distance: {sep_distance_slider.value}"
            sep_distance_label.fit_content()
            self.sep_distance = sep_distance_slider.value

        posX = -screen_size[0]//2 + self.sliderW // 2
        posY = -screen_size[1]//2 + self.sliderH//2
        self.manager.add(UIAnchorWidget(child=align_slider, align_x=posX, align_y=posY))
        self.manager.add(UIAnchorWidget(child=align_label, align_x=posX, align_y=posY+20))

        posY += self.sliderH
        self.manager.add(UIAnchorWidget(child=sep_slider, align_x=posX, align_y=posY))
        self.manager.add(UIAnchorWidget(child=sep_label, align_x=posX, align_y=posY+20))

        posY += self.sliderH
        self.manager.add(UIAnchorWidget(child=coh_slider, align_x=posX, align_y=posY))
        self.manager.add(UIAnchorWidget(child=coh_label, align_x=posX, align_y=posY+20))

        posY += self.sliderH
        self.manager.add(UIAnchorWidget(child=percept_slider, align_x=posX, align_y=posY))
        self.manager.add(UIAnchorWidget(child=percept_label, align_x=posX, align_y=posY+20))

        posY += self.sliderH
        self.manager.add(UIAnchorWidget(child=sep_distance_slider, align_x=posX, align_y=posY))
        self.manager.add(UIAnchorWidget(child=sep_distance_label, align_x=posX, align_y=posY+20))


        buffer_format = "4f 4x4 4f"
        # Generate the initial data that we will put in buffer 1.
        initial_data = self.gen_initial_data()

        self.ssbo_1 = self.ctx.buffer(data=array('f', initial_data))
        self.ssbo_2 = self.ctx.buffer(reserve=self.ssbo_1.size)

        # Attribute variable names for the vertex shader
        attributes = ["in_vertex", "in_color"]
        self.vao_1 = self.ctx.geometry(
            [BufferDescription(self.ssbo_1, buffer_format, attributes)],
            mode=self.ctx.POINTS,
        )
        self.vao_2 = self.ctx.geometry(
            [BufferDescription(self.ssbo_2, buffer_format, attributes)],
            mode=self.ctx.POINTS,
        )


        file = open("shaders/compute_shader.glsl")
        compute_shader_source = file.read()
        file = open("shaders/vertex_shader.glsl")
        vertex_shader_source = file.read()
        file = open("shaders/fragment_shader.glsl")
        fragment_shader_source = file.read()
        file = open("shaders/geometry_shader.glsl")
        geometry_shader_source = file.read()

        compute_shader_source = compute_shader_source.replace("COMPUTE_SIZE_X",
                                                              str(self.group_x))
        compute_shader_source = compute_shader_source.replace("COMPUTE_SIZE_Y",
                                                              str(self.group_y))
        self.compute_shader = self.ctx.compute_shader(source=compute_shader_source)

        # Program for visualizing the balls
        self.program = self.ctx.program(
            vertex_shader=vertex_shader_source,
            geometry_shader=geometry_shader_source,
            fragment_shader=fragment_shader_source,
        )

        # Enable timings for the performance graph
        arcade.enable_timings()

        # Create a sprite list to put the performance graph into
        self.perf_graph_list = arcade.SpriteList()

        # Create the FPS performance graph
        graph = arcade.PerfGraph(GRAPH_WIDTH, GRAPH_HEIGHT, graph_data="FPS")
        graph.center_x = GRAPH_WIDTH / 2
        graph.center_y = self.height - GRAPH_HEIGHT / 2
        self.perf_graph_list.append(graph)
        self.screen_size = self.get_size()
        self.compute_shader["screen_size"] = self.screen_size

    def on_draw(self):
        self.clear((60,60,60))
        self.ctx.enable(self.ctx.BLEND)

        # Bind buffers
        self.ssbo_1.bind_to_storage_buffer(binding=0)
        self.ssbo_2.bind_to_storage_buffer(binding=1)

        self.compute_shader['factors'] = (self.coherence, self.separation, self.alignment, self.sep_distance)
        self.compute_shader['perceptionDistance'] = self.perception 
        self.compute_shader['mouse'] = (self.x, self.y)
        self.compute_shader['mouseHeldR'] = self.mouseHeldR
        self.compute_shader['mouseHeldL'] = self.mouseHeldL

        # Run compute shader
        self.compute_shader.run(group_x=self.group_x, group_y=self.group_y)

        # Draw the balls
        self.vao_2.render(self.program)

        # Swap the buffers around (we are ping-ping rendering between two buffers)
        self.ssbo_1, self.ssbo_2 = self.ssbo_2, self.ssbo_1
        # Swap what geometry we draw
        self.vao_1, self.vao_2 = self.vao_2, self.vao_1

        # Draw the graphs
        self.perf_graph_list.draw()
        self.manager.draw()

    def gen_initial_data(self):
        for i in range(self.num_balls):
            # Position/radius
            yield random.randrange(100, self.width-100)
            yield random.randrange(100, self.height-100)
            yield 0.0  # z (padding)
            yield 6.0

            # Velocity
            yield 0.0
            yield 0.0
            yield 0.0  # vz (padding)
            yield 0.0  # vw (padding)

            # Color
            yield 1.0  # r
            yield 1.0  # g
            yield 1.0  # b
            yield 1.0  # a

    def on_mouse_motion(self, x, y, dx, dy):
        """
        Called whenever the mouse moves.
        """
        self.x = x
        self.y = y

    def on_mouse_press(self, x, y, button, modifiers):
        if x < self.sliderW and y < self.sliderH*6:
            return
        if button == arcade.MOUSE_BUTTON_LEFT:
            self.mouseHeldL = True
        elif button == arcade.MOUSE_BUTTON_RIGHT:
            self.mouseHeldR = True

    def on_mouse_release(self, x, y, button, modifiers):
        if x < self.sliderW and y < self.sliderH*6:
            return
        if button == arcade.MOUSE_BUTTON_LEFT:
            self.mouseHeldL = False
        elif button == arcade.MOUSE_BUTTON_RIGHT:
            self.mouseHeldR = False


app = MyWindow()
arcade.run()