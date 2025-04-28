from kivy.app import App
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from kivy.clock import Clock
from kivy.properties import NumericProperty, StringProperty, ListProperty
from kivy.uix.label import Label
from kivy.uix.widget import Widget
from kivy.graphics import Color, Ellipse, Line
from math import radians, sin, cos
import serial
import threading

KV = '''
<RaceYourself>:
    orientation: 'vertical'
    padding: dp(20)
    spacing: dp(10)

    BoxLayout:
        size_hint_y: None
        height: dp(50)
        Label:
            text: "Top Speed:"
            font_size: '20sp'
            bold: True
        Label:
            text: root.top_speed_text
            font_size: '24sp'
            color: 1,0.5,0,1

    SpeedGauge:
        id: gauge
        size_hint_y: 0.6
        speed: root.current_speed

    BoxLayout:
        size_hint_y: None
        height: dp(40)
        Button:
            text: "New Run"
            on_release: root.start_new_run()
        Label:
            text: root.status_text
            font_size: '16sp'

    BoxLayout:
        orientation: 'vertical'
        Label:
            text: "Run History"
            font_size: '18sp'
            bold: True
        ScrollView:
            size_hint_y: 0.3
            GridLayout:
                id: history_grid
                cols: 1
                size_hint_y: None
                row_default_height: dp(30)
                height: self.minimum_height
                spacing: dp(5)
                padding: dp(5)
'''

class SpeedGauge(Widget):
    speed = NumericProperty(0)

    def on_size(self, *args):
        self.update_canvas()

    def on_pos(self, *args):
        self.update_canvas()

    def on_speed(self, *args):
        self.update_canvas()

    def update_canvas(self):
        self.canvas.clear()
        radius = min(self.width, self.height) / 2 - 10
        cx, cy = self.center

        with self.canvas:
            Color(0.2, 0.2, 0.2)
            Ellipse(pos=(cx - radius, cy - radius), size=(2 * radius, 2 * radius))

            for i in range(0, 181, 20):
                ang = radians(180 - i)
                x1 = cx + (radius - 10) * cos(ang)
                y1 = cy + (radius - 10) * sin(ang)
                x2 = cx + radius * cos(ang)
                y2 = cy + radius * sin(ang)
                Color(1, 1, 1)
                Line(points=[x1, y1, x2, y2], width=1.5)

            angle = radians(180 - (self.speed / 20.0) * 180)
            nx = cx + (radius - 20) * cos(angle)
            ny = cy + (radius - 20) * sin(angle)
            Color(1, 0, 0)
            Line(points=[cx, cy, nx, ny], width=3)

            Color(0, 0, 0)
            Ellipse(pos=(cx - 5, cy - 5), size=(10, 10))


class RaceYourself(BoxLayout):
    distance_m = NumericProperty(1.5)
    current_speed = NumericProperty(0)
    top_speed = NumericProperty(0)
    top_speed_text = StringProperty("0.00 m/s")
    status_text = StringProperty("Waiting...")
    history = ListProperty([])

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.ser = serial.Serial('COM3', 9600, timeout=0.1)
        threading.Thread(target=self.read_loop, daemon=True).start()

    def start_new_run(self):
        self.status_text = "Run started! Waiting for stop..."
        self.current_speed = 0

    def read_loop(self):
        buf = bytearray()
        while True:
            data = self.ser.read(1)
            if data:
                buf += data
                if len(buf) >= 4:
                    idx = buf.find(b'\xAB')
                    if idx != -1 and idx + 3 < len(buf):
                        if buf[idx + 3] == 0xCD:
                            packet = buf[idx:idx + 4]
                            buf = buf[idx + 4:]
                            ticks = (packet[1] << 8) | packet[2]
                            self.process_ticks(ticks)
                        else:
                            buf = buf[idx + 1:]

    def process_ticks(self, ticks):
        t_sec = ticks / 16_000_000.0
        speed = self.distance_m / t_sec
        self.current_speed = speed
        self.status_text = f"{speed:.2f} m/s"

        self.history.insert(0, f"{speed:.2f} m/s")
        if speed > self.top_speed:
            self.top_speed = speed
            self.top_speed_text = f"{speed:.2f} m/s"

        Clock.schedule_once(lambda dt: self.update_history())

    def update_history(self):
        grid = self.ids.history_grid
        grid.clear_widgets()
        for run in self.history[:10]:
            grid.add_widget(Label(text=run, font_size='16sp'))


class SpeedApp(App):
    def build(self):
        Builder.load_string(KV)
        self.title = "Race Yourself"
        return RaceYourself()


if __name__ == '__main__':
    SpeedApp().run()
