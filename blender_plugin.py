bl_info = {
    "name": "LevelStreaming Editor",
    "category": "System",
    "author": "Matt Filer",
    "description": "Tools for developing custom levels for LevelStreaming project.",
}

import bpy
import json
import numpy as np
from bpy.types import Panel
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty


# The main UI panel for our tools
class LevelStreaming_ToolPanel(Panel):
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'TOOLS'
    bl_label = 'LevelStreaming Tools'
    bl_context = 'objectmode'
    bl_category = 'LevelStreaming Tools'
    
    def draw(self,context):
        layout = self.layout
        layout.operator(LevelStreaming_SpawnMarker.bl_idname, text='Set Player Spawn', icon='CURSOR')
        layout.operator(LevelStreaming_AddZone.bl_idname, text='Add Zone', icon='SNAP_VOLUME')
        layout.operator(LevelStreaming_ExportConfig.bl_idname, text='Save Config', icon='SAVE_COPY')
        
        
# Add a spawn marker (one per map)
class LevelStreaming_SpawnMarker(bpy.types.Operator):
    """Add a New Spawn Marker"""
    bl_idname = "object.levelstreaming_spawnmarker_add"
    bl_label = "Spawn Position"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        for item in bpy.context.selectable_objects:  
            item.select = False  
        
        #Create spawn spot object
        player_spawn = bpy.data.objects.new(name="Player Spawn Position", object_data=None)
        bpy.context.scene.objects.link(player_spawn)
        player_spawn.location = (0,0,0)
        
        #Set to selected object's position if we can
        if bpy.context.active_object != None:
            player_spawn.location = bpy.context.scene.objects.active.location
            
        #Select object
        player_spawn.select = True
        bpy.context.scene.objects.active = player_spawn
        
        return {'FINISHED'}
        
        
# Add a marker for the streaming zone
class LevelStreaming_AddZone(bpy.types.Operator):
    """Add a Zone Marker"""
    bl_idname = "object.levelstreaming_newzone_add"
    bl_label = "Streaming Zone"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        for item in bpy.context.selectable_objects:  
            item.select = False  
            
            
        #Create a new cube
        bpy.ops.mesh.primitive_cube_add()
        cube = bpy.context.selected_objects[0]
        
        #Change its name
        cube.name = "LevelStreamingZone"
        
        #Set to selected object's position if we can
        if bpy.context.active_object != None:
            cube.location = bpy.context.scene.objects.active.location
        
        return {'FINISHED'}
        
        
# Save all config content
class LevelStreaming_ExportConfig(bpy.types.Operator, ExportHelper):
    """Save LevelStreaming Config"""
    bl_idname = "export_test.levelstreaming_export"
    bl_label = "Save LevelStreaming Config"
    
    filename_ext = ".json"

    filter_glob = StringProperty(
            default="*.json",
            options={'HIDDEN'},
            maxlen=255,  # Max internal buffer length, longer would be clamped.
            )

    def execute(self, context):
        #JSON template
        LevelStreaming_json = {'cams': [], 'look_at_points': [], 'spawns': [], 'waypoints': [], 'finish_line': [], 'item_boxes': [], 'glider_track': []}
        waypoint_index = 0
        
        for object in bpy.data.objects:
            if object.type == "EMPTY":
                #Kart spawn position
                if object.name[:19] == "Kart Spawn Position":
                    LevelStreaming_json["spawns"].append({
                        "name": object.name, 
                        "pos": [object.location[0], object.location[1], object.location[2]], 
                        "rotation": [object.rotation_euler[0], object.rotation_euler[1], object.rotation_euler[2]]
                    })
                #Item box position
                if object.name[:14] == "Item Box Spawn":
                    LevelStreaming_json["item_boxes"].append({
                        "pos": [object.location[0], object.location[1], object.location[2]], 
                        "rotation": [object.rotation_euler[0], object.rotation_euler[1], object.rotation_euler[2]]
                    })
                #Look-at position
                if object.name[:17] == "Intro Cam Look-At":
                    LevelStreaming_json["look_at_points"].append({
                        "index": object.name[-1:],
                        "pos": [object.location[0], object.location[1], object.location[2]]
                    })
                    
            if object.type == "CAMERA":
                #Cinematic cam 
                LevelStreaming_json["cams"].append({
                    "index": object.name[-1:],
                    "role": object.name[19:-2], 
                    "pos": [object.location[0], object.location[1], object.location[2]]
                })
                
            if object.type == "MESH":
                #Track waypoint
                if object.name[:21] == "Track Waypoint Marker":
                    if len(object.data.vertices) == 4:
                        top_left = object.matrix_world * object.data.vertices[2].co;
                        top_right = object.matrix_world * object.data.vertices[3].co;
                        bottom_left = object.matrix_world * object.data.vertices[0].co;
                        bottom_right = object.matrix_world * object.data.vertices[1].co;
                        LevelStreaming_json["waypoints"].append({
                            "index": waypoint_index,
                            "top_left": [top_left[0], top_left[1], top_left[2]],
                            "top_right": [top_right[0], top_right[1], top_right[2]],
                            "bottom_left": [bottom_left[0], bottom_left[1], bottom_left[2]],
                            "bottom_right": [bottom_right[0], bottom_right[1], bottom_right[2]]
                        })
                        waypoint_index += 1
                #Finish line marker
                if object.name[:18] == "Finish Line Marker":
                    if len(object.data.vertices) == 4:
                        top_left = object.matrix_world * object.data.vertices[2].co;
                        top_right = object.matrix_world * object.data.vertices[3].co;
                        bottom_left = object.matrix_world * object.data.vertices[0].co;
                        bottom_right = object.matrix_world * object.data.vertices[1].co;
                        LevelStreaming_json["finish_line"].append({
                            "top_left": [top_left[0], top_left[1], top_left[2]],
                            "top_right": [top_right[0], top_right[1], top_right[2]],
                            "bottom_left": [bottom_left[0], bottom_left[1], bottom_left[2]],
                            "bottom_right": [bottom_right[0], bottom_right[1], bottom_right[2]]
                        })
                #Glider track
                if object.name[:12] == "Glider Track":
                    if len(object.data.vertices) == 4:
                        top_left = object.matrix_world * object.data.vertices[2].co;
                        top_right = object.matrix_world * object.data.vertices[3].co;
                        bottom_left = object.matrix_world * object.data.vertices[0].co;
                        bottom_right = object.matrix_world * object.data.vertices[1].co;
                        LevelStreaming_json["glider_track"].append([
                            [top_left[0], top_left[1], top_left[2]], #Triangle1: top_left
                            [bottom_left[0], bottom_left[1], bottom_left[2]], #Triangle1: bottom_left
                            [bottom_right[0], bottom_right[1], bottom_right[2]] #Triangle1: bottom_right
                        ])
                        LevelStreaming_json["glider_track"].append([
                            [top_left[0], top_left[1], top_left[2]], #Triangle2: top_left
                            [top_right[0], top_right[1], top_right[2]], #Triangle2: top_right
                            [bottom_right[0], bottom_right[1], bottom_right[2]] #Triangle2: bottom_right
                        ])
        
        #Save out to file
        with open(self.filepath, 'w') as outfile:
            json.dump(LevelStreaming_json, outfile, indent=2, sort_keys=True)
        
        return {'FINISHED'}
    
    
# Add menu functions
def menu_func(self, context):
    self.layout.operator(LevelStreaming_SpawnMarker.bl_idname)
    self.layout.operator(LevelStreaming_AddZone.bl_idname)
    
def menu_func_export(self, context):
    self.layout.operator(LevelStreaming_ExportConfig.bl_idname)
    
    
# Register plugin on load
def register():
    bpy.utils.register_class(LevelStreaming_ToolPanel)
    bpy.utils.register_class(LevelStreaming_SpawnMarker)
    bpy.utils.register_class(LevelStreaming_AddZone)
    bpy.utils.register_class(LevelStreaming_ExportConfig)
    bpy.types.VIEW3D_MT_object.append(menu_func)
    bpy.types.INFO_MT_file_export.append(menu_func_export)
    
# Unregister on exit
def unregister():
    bpy.utils.unregister_class(LevelStreaming_ToolPanel)
    bpy.utils.unregister_class(LevelStreaming_SpawnMarker)
    bpy.utils.unregister_class(LevelStreaming_AddZone)
    bpy.utils.unregister_class(LevelStreaming_ExportConfig)
    bpy.types.VIEW3D_MT_object.remove(menu_func)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)
    
    
if __name__ == '__main__':
    register()