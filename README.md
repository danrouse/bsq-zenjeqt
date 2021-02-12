# Zenjeqt

Zenject's dependency injection for Quest! Constructor, method, and field injection are supported (parameter injection is not implemented).

## Basic guide

In general, usage patterns should follow how Zenject is used in PC mods. Notably, SiraUtil's Zenjector class is mostly ported, so those entry points may be used.
_(TODO: expand on this bit with specifics, once the API is more hardened)_

1. In your custom-types classes, use the `REGISTER_X_INJECT` macros to mark fields or methods to be injected,
2. Create an `Installer` class with an `InstallBindings` method,
3. In `InstallBindings`, call Zenject's binding methods like normal,
4. Register the `Installer` with using Zenjeqtor's `OnX` methods, such as `OnApp`, `OnMenu`, or `OnGame`.

For basic examples, see the implementations in [this repo's `test` directory](test/).

For more detailed information on Zenject itself, check out [the SiraUtil README](https://github.com/Auros/SiraUtil/blob/master/README.md#zenject).

## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario)
* [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)
