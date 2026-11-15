import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import media_player
from esphome.const import CONF_TRIGGER_ID

CONF_ON_PLAY_URL = "on_play_url"

answer_relay_ns = cg.esphome_ns.namespace("answer_relay")
AnswerRelay = answer_relay_ns.class_("AnswerRelay", media_player.MediaPlayer, cg.Component)
PlayTrigger = answer_relay_ns.class_("PlayTrigger", automation.Trigger.template(cg.std_string))

CONFIG_SCHEMA = media_player.media_player_schema(AnswerRelay).extend(
    {
        cv.Required(CONF_ON_PLAY_URL): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PlayTrigger)}
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await media_player.new_media_player(config)
    await cg.register_component(var, config)
    for conf in config[CONF_ON_PLAY_URL]:
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "x")], conf)
