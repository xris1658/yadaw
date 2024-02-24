# Conventions of defining and implementing Models used in YADAW

## Defining Models
- Define your Model in `YADAW::Model` and register it to `YADAW.Models`.
- Derive your Model from `QAbstractListModel`, or `ISortFilterListModel` if the 
  Model is used with `ISortFilterProxyListModel`.
- Provide a constructor, virtual destructor and a public `static constexpr int
  columnCount()`.

## Naming convention
- Class names: `***ListModel` or `***Model` in PascalCase
- Roles: PascalCase
- Role names: `***_xxx` in snake_case, in which
  - `***` is the upper case letters (without the first letter I) of the class
    name, in lower case.
  - `xxx` is the role in lower case.

For example, class `IAssetDirectoryListModel` has its roles and role names as:
- `Id` named `adlm_id`
- `Path` named `adlm_path`
- `Name` named `adlm_name`

## Roles
- Define roles using nested unscoped `enum Role`.
  - Always assign the first enumerator with `Qt::UserRole`.
  - Always end the enumerators with `RoleCount`.
  - Add `Q_ENUM(Role)` if the role is needed in QML codes.
  - With roles defined as such, add a member function
    ```cpp
    public:
        static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    ```

## Some member functions
Define `columnCount()` and `columnCount(const QModelIndex&)` as follows:
```cpp
public:
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final
    {
        return columnCount();
    }
```