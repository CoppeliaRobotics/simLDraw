function sysCall_info()
    return {autoStart = false, menu = 'Importers\nLDraw importer...'}
end

function sysCall_init()
    local sim = require 'sim'
    local simUI = require 'simUI'
    local simLDraw = require 'simLDraw'
    local scenePath = sim.getStringParam(sim.stringparam_scene_path)
    local fileNames = simUI.fileDialog(
                         simUI.filedialog_type.load, 'Open LDraw file...', scenePath, '',
                         'LDraw files (*.ldr; *.dat; *.mpd)', 'ldr;dat;mpd', true
                     )
    if #fileNames > 0 then simLDraw.import(fileNames[1]) end
    return {cmd = 'cleanup'}
end
