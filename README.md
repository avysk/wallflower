## Search architecture

Concrete classes, doing searches (say, `WallheavenSearcher`) should implement
two methods:

- `searchWallpapers`
- `attach(searcher, `*searcher's search finished slot*`, `*searcher's search
  error slot*`)`. When the search is finished, searcher's search finished slot
  should be called with `QVector<Picture> &result`, and when there was an error
  in the search, searcher's error slot should be called with `QString &message`.
  To do that, two signals (`searchFinished` and `searchError`) are created and
  wired appropriately in `attach`.

The constructor `BaseSearcher<SearcherImplementation, Searcher>` creates
`std::unique_ptr<SearcherImplementation> mImpl` field and does the wiring
with `mImpl->attach`. The class `BaseSearcher<SearcherImplementation, Searcher>`
inherits from the class `SearcherInterface`, so its instances can be put inside
`Searcher` into `std::unique_ptr<SearcherInterface> mImpl` field. The `Searcher`
objects also have `setImpl` method, allowing to change the content of `mImpl`
field.

### Initialization

1. `Searcher` instance created.
2. For suitable `SearcherImplementation` the instance of
   `BaseSearcher<SearcherImplementation, Searcher>` is created and pointer to
   it is stored in `Searcher`'s instance `mImpl' field.
3. When the instance of `BaseSearcher<SearcherImplementation, Searcher>` is
   created, the instance of `SearcherImplementation` is created and stored in
   `BaseSearcher`'s instance `mImpl` field; also `mImpl->attach` is called,
   wiring the `SearcherImplementation`'s signals and `Searcher`' slots.

### Search

1. `Searcher`'s `searchWallpapers` method is called.
2. It calls defined in `searcherInterface` method `mImpl->searchWallpapers`.
3. When `searchWallpapers` method of `BaseSearcher` is called, it calls
   `mImpl->searchWallpapers` method of underlying `SearcherImplementation`
   instance.
4. When the search terminates or errors out, the instance of
   `SearcherImplementation` emits a signal, which was previosly wired to a slot
   of the `Searcher` instance.

### Concepts

- `IsSearcher<Searcher>` makes sure that slots `onSearchFinished` and
  `onSearchError` exist.
- `IsSearcherImplementation<SearcherImplementation, Searcher>` makes sure that
  methods `searchWallpapers` and `attach` exist. The second template parameter
  is needed because `attach` signature depends on it.

### Dependencies

Searcher -> SearcherInterface
BaseSearcher -> SearcherInterface
SearcherImplementation -> Searcher
